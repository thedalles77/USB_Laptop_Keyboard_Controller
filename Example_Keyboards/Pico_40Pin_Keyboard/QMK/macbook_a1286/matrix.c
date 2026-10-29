// Copyright 2026 Frank Adams (@thedalles77)
// SPDX-License-Identifier: GPL-2.0-or-later

// Custom matrix scan for the Pi Pico + MCP23018 laptop keyboard controller.
//
// Pin numbering matches the Arduino sketches:
//   0 - 29    : Raspberry Pi Pico GPIO number (GP0 - GP29)
//   100 - 107 : MCP23018 GPA0 - GPA7
//   108 - 115 : MCP23018 GPB0 - GPB7
//
// Scanning: one row at a time is driven low, all other rows and all columns
// are inputs with pull-ups. A pressed key pulls its column low. The MCP23018
// outputs are open drain and the Pico rows are released back to inputs, so
// there is never a path to 3.3V through multiple pressed keys.

#include "matrix.h"
#include "i2c_master.h"
#include "gpio.h"
#include "wait.h"

// ---- Keyboard specific pin lists (same as the TinyUSB sketch) ----
static const uint8_t row_pins[MATRIX_ROWS] = {101, 20, 112, 9, 102, 10, 103, 19, 111, 11, 110, 18, 105, 14, 107, 15};
static const uint8_t col_pins[MATRIX_COLS] = {100, 7, 113, 8, 17, 109, 16, 106, 108};

// ---- MCP23018 ----
#define MCP23018_ADDR (0x20 << 1) // QMK I2C functions take the 8-bit (shifted) address
#define MCP_IODIRA 0x00           // direction: 1 = input, 0 = output (A then B with IOCON.BANK = 0)
#define MCP_GPPUA 0x0C            // internal pull-ups
#define MCP_GPIOA 0x12            // port read
#define MCP_OLATA 0x14            // output latch
#define MCP_TIMEOUT 20            // ms

#define EXP_BASE 100
#define IS_EXP(p) ((p) >= EXP_BASE)
#define EXP_BIT(p) ((uint16_t)1 << ((p) - EXP_BASE))

#define SETTLE_US 20 // same settle delay as the Arduino code

static bool mcp_ok = false;

static bool mcp_write16(uint8_t reg, uint16_t value) {
    uint8_t data[2] = {value & 0xFF, value >> 8}; // GPA byte first, then GPB
    return i2c_write_register(MCP23018_ADDR, reg, data, 2, MCP_TIMEOUT) == I2C_STATUS_SUCCESS;
}

static bool mcp_read16(uint8_t reg, uint16_t *value) {
    uint8_t data[2];
    if (i2c_read_register(MCP23018_ADDR, reg, data, 2, MCP_TIMEOUT) != I2C_STATUS_SUCCESS) {
        return false;
    }
    *value = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    return true;
}

static bool mcp_init(void) {
    // Output latches are always 0. A pin is driven low simply by
    // switching it to an output, and released by switching it back to an input.
    return mcp_write16(MCP_OLATA, 0x0000) && mcp_write16(MCP_IODIRA, 0xFFFF) && mcp_write16(MCP_GPPUA, 0xFFFF);
}

void matrix_init_custom(void) {
    // Pico rows and columns: inputs with internal (~50K) pull-ups
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (!IS_EXP(row_pins[r])) gpio_set_pin_input_high(row_pins[r]);
    }
    for (uint8_t c = 0; c < MATRIX_COLS; c++) {
        if (!IS_EXP(col_pins[c])) gpio_set_pin_input_high(col_pins[c]);
    }

    i2c_init();
    wait_ms(10); // let the MCP23018 come out of reset (it shares the Pico RUN line)
    mcp_ok = mcp_init();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool changed = false;

    // If the expander didn't answer at power-up, keep trying (e.g. slow power-up)
    if (!mcp_ok) {
        mcp_ok = mcp_init();
        if (!mcp_ok) return false;
    }

    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        uint8_t row = row_pins[r];

        // --- Drive this row low ---
        if (IS_EXP(row)) {
            if (!mcp_write16(MCP_IODIRA, (uint16_t)~EXP_BIT(row))) {
                mcp_ok = false;
                return changed;
            }
        } else {
            gpio_write_pin_low(row);
            gpio_set_pin_output(row);
        }

        wait_us(SETTLE_US);

        // --- Read all columns ---
        uint16_t exp_pins = 0xFFFF;
        bool     read_ok  = mcp_read16(MCP_GPIOA, &exp_pins);

        matrix_row_t row_state = 0;
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            uint8_t col     = col_pins[c];
            bool    pressed = IS_EXP(col) ? !(exp_pins & EXP_BIT(col)) : !gpio_read_pin(col);
            if (pressed) row_state |= (matrix_row_t)1 << c;
        }

        // --- Release the row ---
        if (IS_EXP(row)) {
            mcp_write16(MCP_IODIRA, 0xFFFF);
        } else {
            gpio_set_pin_input_high(row);
        }

        // On an I2C error keep the old row state rather than reporting bogus keys
        if (!read_ok) {
            mcp_ok = false;
            return changed;
        }

        if (current_matrix[r] != row_state) {
            current_matrix[r] = row_state;
            changed           = true;
        }
    }

    return changed;
}
