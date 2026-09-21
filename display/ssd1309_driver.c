#include "ssd1309_driver.h"

#include <hardware/gpio.h>
#include <hardware/spi.h>

#include <stdint.h>

#define SPI_SYS spi0
#define BAUDRATE 8000000 // SPI BAUDRATE: 8MHz

#define DC_GPIO 0  // GPIO 0 -> DATA/COMMAND SELECT
#define CS_GPIO 1  // GPIO 1 -> SPI0 CS
#define SCK_GPIO 2 // GPIO 2 -> SPI0 SCK
#define TX_GPIO 3  // GPIO 3 -> SPI0 TX
#define RST_GPIO 4 // GPIO 4 -> RESET

#define SPI_WRITE_LEN 1 // spi_write_byte() byte length (1 byte)

typedef enum {
    CMD_DISP_ON = 0xAF,
    CMD_DISP_OFF = 0xAE,
    CMD_CLK = 0xD5,
    CMD_MUX_RATIO = 0xA8,
    CMD_DISP_OFFSET = 0xD3,
    CMD_START_LINE = 0x40, // START LINE = 0.
    CMD_ADDR_MODE = 0x20,
    CMD_COL_ADDRS = 0x21,
    CMD_PAGE_ADDRS = 0x22,
    CMD_FLIP_H = 0xA1,
    CMD_UNFLIP_H = 0xA0,
    CMD_FLIP_V = 0xC8,
    CMD_UNFLIP_V = 0xC0,
    CMD_COM_PIN_CONF = 0xDA,
    CMD_CONTRAST = 0x81,
    CMD_PRECHRG_PERIOD = 0xD9,
    CMD_VCOMH_DESEL_LVL = 0xDB,
    CMD_ALL_PX_ON = 0xA5,
    CMD_ALL_PX_OFF = 0xA4,
    CMD_INVERT = 0xA7,
    CMD_UNINVERT = 0xA6
} COMMANDS;

typedef enum {
    DEFAULT_CONTRAST = 0x7F,
    DEFAULT_MUX_RATIO = 0x3F,
    DEFAULT_OFFSET = 0x0,
    DEFAULT_ADDR_MODE_H = 0x0,
    DEFAULT_COL_START = 0x0,
    DEFAULT_COL_END = 0x7F,
    DEFAULT_PAGE_START = 0x0,
    DEFAULT_PAGE_END = 0x7,
    DEFAULT_COM_PIN_CONF = 0x12,
    DEFAULT_PRECHRG_PERIOD = 0x22, // TODO: tweak value. hi nib = charge (phase 2), lo nib = discharge (phase 1)
    DEFAULT_CLK = 0x80,            // TODO: tweak value. hi nib = osc freq, lo nib = divider.
    DEFAULT_VCOMH = 0x20           // TODO: tweak value.
} DEFAULTS;

// Abstraction of spi_write_blocking(). Writes one byte to SPI0's Tx buffer.
void spi_write_byte(uint8_t in) {
    spi_write_blocking(SPI_SYS, &in, SPI_WRITE_LEN);
}

void init_ssd1309() {
    /* GPIO INIT */
    spi_init(SPI_SYS, BAUDRATE);

    gpio_set_function_masked((1 << CS_GPIO) | (1 << SCK_GPIO) | (1 << TX_GPIO), GPIO_FUNC_SPI);
    gpio_init_mask((1 << DC_GPIO) | (1 << RST_GPIO));
    gpio_set_dir_masked((1 << DC_GPIO) | (1 << RST_GPIO), GPIO_OUT);

    /* DISPLAY INIT */

    // RST high
    gpio_put(RST_GPIO, true);

    // D/C low (issue commands)
    gpio_put(DC_GPIO, false);

    spi_write_byte(CMD_DISP_OFF);

    spi_write_byte(CMD_CLK);
    spi_write_byte(DEFAULT_CLK);

    spi_write_byte(CMD_MUX_RATIO);
    spi_write_byte(DEFAULT_MUX_RATIO);

    spi_write_byte(CMD_DISP_OFFSET);
    spi_write_byte(DEFAULT_OFFSET);

    spi_write_byte(CMD_START_LINE);

    spi_write_byte(CMD_ADDR_MODE);
    spi_write_byte(DEFAULT_ADDR_MODE_H);

    spi_write_byte(CMD_COL_ADDRS);
    spi_write_byte(DEFAULT_COL_START);
    spi_write_byte(DEFAULT_COL_END);

    spi_write_byte(CMD_PAGE_ADDRS);
    spi_write_byte(DEFAULT_PAGE_START);
    spi_write_byte(DEFAULT_PAGE_END);

    spi_write_byte(CMD_UNFLIP_H);

    spi_write_byte(CMD_UNFLIP_V);

    spi_write_byte(CMD_COM_PIN_CONF);
    spi_write_byte(DEFAULT_COM_PIN_CONF);

    spi_write_byte(CMD_CONTRAST);
    spi_write_byte(DEFAULT_CONTRAST);

    spi_write_byte(CMD_PRECHRG_PERIOD);
    spi_write_byte(DEFAULT_PRECHRG_PERIOD);

    spi_write_byte(CMD_VCOMH_DESEL_LVL);
    spi_write_byte(DEFAULT_VCOMH);

    spi_write_byte(CMD_ALL_PX_OFF);

    spi_write_byte(CMD_UNINVERT);

    spi_write_byte(CMD_DISP_ON);
}

void clear_framebuf() {
    // D/C high (write to display RAM)
    gpio_put(DC_GPIO, true);

    for (int i = 0; i < 1024; i++) {
        framebuf[i] = 0;
        spi_write_byte(0);
    }
}

void write_framebuf() {

    // D/C high (write to display RAM)
    gpio_put(DC_GPIO, true);

    // for each page in display ram
    for (uint8_t i = 0; i < 8; i++) {
        // for each byte in top row of page
        for (uint8_t j = 0; j < 16; j++) {
            // for each bit in byte
            for (uint8_t k = 0; k < 8; k++) {
                // set mask for byte position
                uint8_t clr_mask = (1 << (7 - k));
                uint8_t byte_sent = 0;
                // for each row in page
                for (uint8_t l = 0; l < 8; l++) {
                    // get bit at masked position
                    uint8_t temp = framebuf[((i << 7) + j) + (l << 4)] & clr_mask;
                    // add to byte_sent at iter position
                    if (temp) {
                        byte_sent |= (1 << l);
                    }
                }
                spi_write_byte(byte_sent);
            }
        }
    }
}