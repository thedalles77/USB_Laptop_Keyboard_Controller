#include "matrix.h"
#include "i2c_master.h"
#include "gpio.h"
#include "timer.h"
#include "print.h"

#define MCP23018_ADDR (0x20 << 1) // 7-bit 0x20 shifted left to 8-bit 0x40 for QMK
#define IODIRA 0x00
#define IODIRB 0x01
#define GPIOA  0x12
#define GPIOB  0x13
#define GPPUA  0x0C
#define GPPUB  0x0D

