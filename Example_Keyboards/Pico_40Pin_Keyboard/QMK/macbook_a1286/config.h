#pragma once

#define MATRIX_ROWS 16
#define MATRIX_COLS 9

// Routes native QMK master operations down to Hardware I2C1
#define I2C1_SDA_PIN GP2
#define I2C1_SCL_PIN GP3
#define I2C_DRIVER_I2CD1
// Layout macro to route keys directly down to matrix coordinates [row, column]
#define LAYOUT( \
    k00, k01, k02, k03, k04, k05, k06, k07, k08, \
    k10, k11, k12, k13, k14, k15, k16, k17, k18, \
    k20, k21, k22, k23, k24, k25, k26, k27, k28, \
    k30, k31, k32, k33, k34, k35, k36, k37, k38, \
    k40, k41, k42, k43, k44, k45, k46, k47, k48, \
    k50, k51, k52, k53, k54, k55, k56, k57, k58, \
    k60, k61, k62, k63, k64, k65, k66, k67, k68, \
    k70, k71, k72, k73, k74, k75, k76, k77, k78, \
    k80, k81, k82, k83, k84, k85, k86, k87, k88, \
    k90, k91, k92, k93, k94, k95, k96, k97, k98, \
    kA0, kA1, kA2, kA3, kA4, kA5, kA6, kA7, kA8, \
    kB0, kB1, kB2, kB3, kB4, kB5, kB6, kB7, kB8, \
    kC0, kC1, kC2, kC3, kC4, kC5, kC6, kC7, kC8, \
    kD0, kD1, kD2, kD3, kD4, kD5, kD6, kD7, kD8, \
    kE0, kE1, kE2, kE3, kE4, kE5, kE6, kE7, kE8, \
    kF0, kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8  \
) { \
    { k00, k01, k02, k03, k04, k05, k06, k07, k08 }, \
    { k10, k11, k12, k13, k14, k15, k16, k17, k18 }, \
    { k20, k21, k22, k23, k24, k25, k26, k27, k28 }, \
    { k30, k31, k32, k33, k34, k35, k36, k37, k38 }, \
    { k40, k41, k42, k43, k44, k45, k46, k47, k48 }, \
    { k50, k51, k52, k53, k54, k55, k56, k57, k58 }, \
    { k60, k61, k62, k63, k64, k65, k66, k67, k68 }, \
    { k70, k71, k72, k73, k74, k75, k76, k77, k78 }, \
    { k80, k81, k82, k83, k84, k85, k86, k87, k88 }, \
    { k90, k91, k92, k93, k94, k95, k96, k97, k98 }, \
    { kA0, kA1, kA2, kA3, kA4, kA5, kA6, kA7, kA8 }, \
    { kB0, kB1, kB2, kB3, kB4, kB5, kB6, kB7, kB8 }, \
    { kC0, kC1, kC2, kC3, kC4, kC5, kC6, kC7, kC8 }, \
    { kD0, kD1, kD2, kD3, kD4, kD5, kD6, kD7, kD8 }, \
    { kE0, kE1, kE2, kE3, kE4, kE5, kE6, kE7, kE8 }, \
    { kF0, kF1, kF2, kF3, kF4, kF5, kF6, kF7, kF8 }  \
}
