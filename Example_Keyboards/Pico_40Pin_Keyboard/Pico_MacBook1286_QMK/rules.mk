# MCU development target configuration
MCU = RP2040
BOOTLOADER = rp2040

# Feature implementation
CUSTOM_MATRIX = lite    # Activates custom matrix compilation via matrix.c
I2C_DRIVER = hardware   # Mounts QMK's native ChibiOS I2C driver structure
