#include "matrix.h"
#include "i2c_master.h"
#include "gpio.h"
#include "wait.h"

#define MCP23018_ADDR (0x20 << 1) // QMK I2C driver requires a pre-shifted 8-bit address

// Define unique hardware flag signatures that won't conflict with direct RP2040 pin mappings
#define EXP_PIN(p) ((pin_t)(0x80 | (p)))
#define IS_EXP_PIN(p) (((p) & 0x80) != 0)
#define GET_EXP_PIN_NUM(p) ((p) & 0x7F)

static const pin_t row_pins[16] = { 
    EXP_PIN(1), GP20, EXP_PIN(12), GP9, EXP_PIN(2), GP10, EXP_PIN(3), GP19, 
    EXP_PIN(11), GP11, EXP_PIN(10), GP18, EXP_PIN(5), GP14, EXP_PIN(7), GP15
};

static const pin_t col_pins[9] = { 
    EXP_PIN(0), GP7, EXP_PIN(13), GP8, GP17, EXP_PIN(9), GP16, EXP_PIN(6), EXP_PIN(8)
};

// Local storage macros
#define I2C_TIMEOUT 100

// Helper to write a 16-bit register config package down to the MCP23018 Expander
static bool mcp23018_write_register16(uint8_t reg, uint16_t data) {
    uint8_t data_buffer[2] = { data & 0xFF, (data >> 8) & 0xFF };
    return (i2c_write_reg(MCP23018_ADDR, reg, data_buffer, 2, I2C_TIMEOUT) == STATUS_SUCCESS);
}

// Helper to grab an optimized 16-bit physical logic map from the expander input pins
static uint16_t mcp23018_read_pins(void) {
    uint8_t buffer[2] = {0};
    // 0x12 points directly to the active GPIOA mapping address
    if (i2c_read_reg(MCP23018_ADDR, 0x12, buffer, 2, I2C_TIMEOUT) == STATUS_SUCCESS) {
        return ((uint16_t)buffer[1] << 8) | buffer[0];
    }
    return 0xFFFF; // Fallback to pulling floating lines high if I2C hangs
}

void matrix_init_custom(void) {
    // 1) Initialize the QMK Hardware I2C framework engine
    i2c_init();

    // 2) Configure native RP2040 GPIO pins into clean input floating lines with pullups
    for (uint8_t i = 0; i < 16; i++) {
        if (!IS_EXP_PIN(row_pins[i])) { setPinInputHigh(row_pins[i]); }     
    }
    for (uint8_t i = 0; i < 9; i++) {
        if (!IS_EXP_PIN(col_pins[i])) { setPinInputHigh(col_pins[i]); }
    }

    // 3) Push I2C payloads initializing MCP23018 configuration baselines
    mcp23018_write_register16(0x00, 0xFFFF); // IODIR: Sets all 16 pins as Inputs
    mcp23018_write_register16(0x0C, 0xFFFF); // GPPU: Engages internal 100k pull-ups on all pins
}

bool matrix_scan_custom(void) {
    bool matrix_has_changed = false;

    for (uint8_t r = 0; r < 16; r++) {
        matrix_row_t current_row_state = 0;

        // --- Step A: Drive Target Row Low ---
        pin_t r_pin = row_pins[r];
        if (!IS_EXP_PIN(r_pin)) {
            setPinOutput(r_pin);
            writePinLow(r_pin);
        } else {
            uint8_t pin_shift = GET_EXP_PIN_NUM(r_pin);
            uint16_t mask = ~(1U << pin_shift);
            mcp23018_write_register16(0x00, mask); // IODIR: Drive target output pin
            mcp23018_write_register16(0x14, mask); // OLAT: Bring pin logic down to absolute low
        }

        wait_us(20); // Hold briefly for voltage stabilization on trace matrices

        // --- Step B: Capture Snapshots ---
        uint16_t mcp_snapshot = mcp23018_read_pins();

        // --- Step C: Evaluate Complete Column State ---
        for (uint8_t c = 0; c < 9; c++) {
            pin_t c_pin = col_pins[c];
            bool is_pressed = false;

            if (!IS_EXP_PIN(c_pin)) {
				is_pressed = (readPin(c_pin) == 0);
			} else {
				uint8_t pin_shift = GET_EXP_PIN_NUM(c_pin);
                is_pressed = ((mcp_snapshot & (1U << pin_shift)) == 0);
            }

            if (is_pressed) {
                current_row_state |= (1U << c);
            }
        }

        // --- Step D: Release Row Back to High-Impedance Pullup State ---
        if (!IS_EXP_PIN(r_pin)) {
            setPinInputHigh(r_pin);
        } else {
            mcp23018_write_register16(0x00, 0xFFFF); // Reset all registers back into input mode
            mcp23018_write_register16(0x0C, 0xFFFF); // Ensure pullups stay armed
        }

        // Save row state changes back to QMK kernel framework arrays
        if (matrix_get_row(r) != current_row_state) {
            matrix_set_row(r, current_row_state);
            matrix_has_changed = true;
        }
    }

    return matrix_has_changed;
}
