# Copyright 2026 Frank Adams
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# KMK matrix scanner for the Pi Pico + MCP23018 port expander keyboard controller board.
# It works the same way as the QMK matrix.c for this board:
#
#   Pin numbers:   0 - 28   = Pico GPIO (GP0 - GP28)
#                100 - 107  = MCP23018 GPA0 - GPA7
#                108 - 115  = MCP23018 GPB0 - GPB7
#
#   Only one row is driven low at a time. Every other row and every column is an
#   input with a pull-up, so no pin is ever driven high (safe for a laptop keyboard
#   matrix with no diodes).
#   The MCP23018 output latches are set to 0 once at startup. An expander row is
#   driven low by making it an output (one I2C write) and released by making it an
#   input again (one I2C write).
#   All 16 expander pins are read with one I2C transfer per row.
#
# Copy this file to the CIRCUITPY drive next to code.py.

import board
import digitalio
from keypad import Event as KeyEvent

from kmk.scanners import Scanner

# MCP23018 registers (IOCON.BANK = 0, so each A register is followed by its B register)
_IODIRA = 0x00  # direction: 1 = input, 0 = output
_GPPUA = 0x0C   # pull-ups: 1 = on
_GPIOA = 0x12   # pin read
_OLATA = 0x14   # output latch

_EXP_BASE = 100


class PicoMCP23018MatrixScanner(Scanner):
    def __init__(self, row_pins, col_pins, i2c, address=0x20, debounce_scans=2):
        '''
        row_pins, col_pins: pin numbers (0-28 Pico GPIO, 100-115 MCP23018)
        i2c: busio.I2C on GP3 (SCL) / GP2 (SDA)
        debounce_scans: a key must read the same on this many scans in a row
                        before its change is reported
        '''
        self.row_pins = tuple(row_pins)
        self.col_pins = tuple(col_pins)
        self.len_rows = len(self.row_pins)
        self.len_cols = len(self.col_pins)
        self._key_count = self.len_rows * self.len_cols
        self.address = address
        self.debounce_scans = debounce_scans

        unique = set(self.row_pins) | set(self.col_pins)
        assert len(unique) == self.len_rows + self.len_cols, (
            'Cannot use a pin as both a column and row'
        )

        # I2C: this scanner is the only user of the bus, so keep it locked
        self.i2c = i2c
        while not self.i2c.try_lock():
            pass
        self._wbuf3 = bytearray(3)
        self._reg = bytearray(1)
        self._rbuf = bytearray(2)
        self.mcp_ok = False

        # Pico pins: inputs with pull-ups. None for expander pins.
        self.row_io = [self._pico_input(p) for p in self.row_pins]
        self.col_io = [self._pico_input(p) for p in self.col_pins]

        # Expander bit masks for rows and columns (0 for Pico pins)
        self.row_bit = [self._exp_bit(p) for p in self.row_pins]
        self.col_bit = [self._exp_bit(p) for p in self.col_pins]

        self._mcp_init()

        # Debounced key state (1 = pressed), debounce counters and pending events
        self.state = bytearray(self._key_count)
        self.count = bytearray(self._key_count)
        self.events = []

    @property
    def key_count(self):
        return self._key_count

    # ----- helpers -----------------------------------------------------------
    @staticmethod
    def _exp_bit(pin):
        return (1 << (pin - _EXP_BASE)) if pin >= _EXP_BASE else 0

    @staticmethod
    def _pico_input(pin):
        if pin >= _EXP_BASE:
            return None
        io = digitalio.DigitalInOut(getattr(board, 'GP' + str(pin)))
        io.switch_to_input(pull=digitalio.Pull.UP)
        return io

    def _mcp_write16(self, reg, value):
        self._wbuf3[0] = reg
        self._wbuf3[1] = value & 0xFF          # GPA byte
        self._wbuf3[2] = (value >> 8) & 0xFF   # GPB byte
        self.i2c.writeto(self.address, self._wbuf3)

    def _mcp_read_pins(self):
        self._reg[0] = _GPIOA
        self.i2c.writeto_then_readfrom(self.address, self._reg, self._rbuf)
        return self._rbuf[0] | (self._rbuf[1] << 8)

    def _mcp_init(self):
        try:
            self._mcp_write16(_OLATA, 0x0000)  # latches stay 0: output = low
            self._mcp_write16(_IODIRA, 0xFFFF)  # all inputs
            self._mcp_write16(_GPPUA, 0xFFFF)   # all pull-ups on
            self.mcp_ok = True
        except OSError:
            self.mcp_ok = False

    # ----- scanning -----------------------------------------------------------
    def _scan_matrix(self):
        '''Reads the raw state of every key. Returns a bytearray (1 = pressed) or None on an I2C error.'''
        raw = bytearray(self._key_count)
        idx = 0
        for r in range(self.len_rows):
            # Drive this row low
            if self.row_io[r] is None:
                self._mcp_write16(_IODIRA, 0xFFFF & ~self.row_bit[r])
            else:
                self.row_io[r].switch_to_output(value=False)

            # Read the expander pins (the I2C transfer also gives the row time to settle),
            # then the Pico pins. An I2C error here is handled in scan_for_changes(),
            # which releases every row.
            exp = self._mcp_read_pins()
            for c in range(self.len_cols):
                bit = self.col_bit[c]
                if bit:
                    pressed = not (exp & bit)
                else:
                    pressed = not self.col_io[c].value
                raw[idx] = 1 if pressed else 0
                idx += 1

            # Release the row: back to an input with pull-up
            if self.row_io[r] is None:
                self._mcp_write16(_IODIRA, 0xFFFF)
            else:
                self.row_io[r].switch_to_input(pull=digitalio.Pull.UP)
        return raw

    def _release_all_rows(self):
        for io in self.row_io:
            if io is not None:
                io.switch_to_input(pull=digitalio.Pull.UP)
        try:
            self._mcp_write16(_IODIRA, 0xFFFF)
        except OSError:
            self.mcp_ok = False

    def scan_for_changes(self):
        '''
        KMK calls this every loop. Returns one key event (or None).
        A full matrix scan is done when no events are waiting.
        '''
        if not self.events:
            if not self.mcp_ok:
                self._mcp_init()  # expander didn't answer: keep trying
                if not self.mcp_ok:
                    return None
            try:
                raw = self._scan_matrix()
            except OSError:
                # I2C error: keep the last good key state and try again next time
                self.mcp_ok = False
                self._release_all_rows()
                return None

            for k in range(self._key_count):
                if raw[k] != self.state[k]:
                    self.count[k] += 1
                    if self.count[k] >= self.debounce_scans:
                        self.state[k] = raw[k]
                        self.count[k] = 0
                        self.events.append(KeyEvent(k + self.offset, raw[k] == 1))
                else:
                    self.count[k] = 0

        if self.events:
            return self.events.pop(0)
        return None
