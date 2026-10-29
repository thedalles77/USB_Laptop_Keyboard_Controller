# macbook_a1286

MacBook Pro A1286 laptop keyboard driven by a Raspberry Pi Pico with an MCP23018
I2C port expander (16 extra GPIOs), using the Pi_Pico_Port_Expander board from
https://github.com/thedalles77/USB_Laptop_Keyboard_Controller

* Keyboard Maintainer: [Frank Adams](https://github.com/thedalles77)
* Hardware Supported: Pi Pico 40-pin port expander board

Build:

    qmk compile -kb macbook_a1286 -km default

## Bootloader

* Hold the Pico's BOOTSEL button while plugging in USB, or
* Double-tap the reset button wired to J1, or
* Hold the Q key (matrix position 0,0) while plugging in USB (bootmagic)
