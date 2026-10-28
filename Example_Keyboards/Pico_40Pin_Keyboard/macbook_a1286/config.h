#pragma once

// Routes native QMK internal operations cleanly down to Hardware I2C1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3
#define I2C_DRIVER_I2CD1
