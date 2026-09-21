#ifndef DISP_DRIVER_H
#define DISP_DRIVER_H

#include <stdint.h>

// FIXME: Move defines to ssd1309_driver.c
// Remember, the .h file is the public interface. 
// Anything that isn't needed by other files goes in the .c file

#define DISP_SPI_SYS spi0
#define DISP_BAUDRATE 8000000 // SPI BAUDRATE: 8MHz

#define DISP_DC_GPIO 0  // GPIO 0 -> DATA/COMMAND SELECT
#define DISP_CS_GPIO 1  // GPIO 1 -> SPI0 CS
#define DISP_SCK_GPIO 2 // GPIO 2 -> SPI0 SCK
#define DISP_TX_GPIO 3  // GPIO 3 -> SPI0 TX
#define DISP_RST_GPIO 4 // GPIO 4 -> RESET

#define DISP_SPI_WRITE_LEN 1 // spi_write_byte() byte length (1 byte)

#define DISP_DEFAULT_CONTRAST 0x7F // 127
#define DISP_MUX_RATIO 0x3F
#define DISP_OFFSET 0x0
#define DISP_ADDR_MODE_H 0x0
#define DISP_COL_START 0x0
#define DISP_COL_END 0x7F
#define DISP_PAGE_START 0x0
#define DISP_PAGE_END 0x7
#define DISP_COM_PIN_CONF 0x12
#define DISP_PRECHRG_PERIOD 0x22

#define CMD_DISP_ON 0xAF
#define CMD_DISP_OFF 0xAE
#define CMD_CLK 0xD5
#define CMD_MUX_RATIO 0xA8
#define CMD_DISP_OFFSET 0xD3
#define CMD_START_LINE 0x40 // START LINE SET TO 0.
#define CMD_ADDR_MODE 0x20
#define CMD_COL_ADDRS 0x21
#define CMD_PAGE_ADDRS 0x22
#define CMD_FLIP_H 0xA1
#define CMD_UNFLIP_H 0xA0
#define CMD_FLIP_V 0xC8
#define CMD_UNFLIP_V 0xC0
#define CMD_COM_PIN_CONF 0xDA
#define CMD_CONTRAST 0x81
#define CMD_PRECHRG_PERIOD 0xD9
#define CMD_VCOMH_DESEL_LVL 0xDB
#define CMD_ALL_PX_ON 0xA5
#define CMD_ALL_PX_OFF 0xA4
#define CMD_INVERT 0xA7
#define CMD_UNINVERT 0xA6

uint8_t framebuf[1024];

/*
Initializes the SSD1309 OLED display and GPIO pins on the Pico.
GPIO 0: Display Data/Command Select (D/C).
GPIO 1: SPI Chip Select.
GPIO 2: SPI Clock.
GPIO 3: SPI Tx.
GPIO 4: Display Reset.

Commanded baudrate: 8MHz.
*/
void init_ssd1309();

/*
Initializes the contents of the framebuffer and display RAM to 0. 
*/
void clear_framebuf();

/*
Write contents of framebuffer to display RAM.
*/
void write_framebuf();

#endif