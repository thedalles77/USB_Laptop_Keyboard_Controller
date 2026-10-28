#pragma once

#define MATRIX_ROWS 16
#define MATRIX_COLS 9

// Routes native QMK master operations down to Hardware I2C1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3
#define I2C_DRIVER_I2CD1
