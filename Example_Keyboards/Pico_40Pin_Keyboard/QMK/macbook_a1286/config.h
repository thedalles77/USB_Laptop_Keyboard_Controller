// Copyright 2026 Frank Adams (@thedalles77)
// SPDX-License-Identifier: GPL-2.0-or-later

// Pico + MCP23018 laptop keyboard controller - MacBook Pro A1286
#pragma once

// MCP23018 port expander is on the Pico's I2C1 peripheral (GP2 = SDA, GP3 = SCL)
#define I2C_DRIVER I2CD1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3
#define I2C1_CLOCK_SPEED 400000

// Double-tap the Pico's RUN/reset button (J1) to enter the UF2 bootloader
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 500U

// Keyboard backlight: GP4 PWM -> gate of BS170 NFET Q1 (J6). GP4 = PWM slice 2, channel A.
#define BACKLIGHT_PWM_DRIVER PWMD2
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_A
