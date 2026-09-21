#ifndef DISP_DRIVER_H
#define DISP_DRIVER_H

#include <stdint.h>

uint8_t framebuf[1024];

/*
Initializes the SSD1309 OLED display and associated GPIO.

GPIO 0: Display Data/Command Select (D/C).
GPIO 1: SPI Chip Select.
GPIO 2: SPI Clock.
GPIO 3: SPI Tx.
GPIO 4: Display Reset.

Commanded baudrate: 8MHz.
*/
void init_ssd1309();


// Initializes the contents of the framebuffer and display RAM to 0.
void clear_framebuf();


// Write contents of framebuffer to display RAM.
void write_framebuf();

#endif