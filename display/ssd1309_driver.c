#include "ssd1309_driver.h"

#include <hardware/gpio.h>
#include <hardware/spi.h>

#include <stdint.h>

/*
Abstraction of spi_write_blocking(). Writes one byte to SPI0's Tx buffer.
*/
void spi_write_byte(uint8_t in) {
    spi_write_blocking(DISP_SPI_SYS, &in, DISP_SPI_WRITE_LEN);
}

void init_ssd1309() {
    /* GPIO INIT */
    spi_init(DISP_SPI_SYS, DISP_BAUDRATE);
    gpio_set_function_masked((1 << DISP_CS_GPIO) | (1 << DISP_SCK_GPIO) | (1 << DISP_TX_GPIO), GPIO_FUNC_SPI);
    
    gpio_init_mask((1 << DISP_DC_GPIO) | (1 << DISP_RST_GPIO));

    gpio_set_dir_masked((1 << DISP_DC_GPIO) | (1 << DISP_RST_GPIO), GPIO_OUT);

    /* DISPLAY INIT */
    // RST high (enable display)
    gpio_put(DISP_RST_GPIO, true);

    // D/C low (issue commands)
    gpio_put(DISP_DC_GPIO, false);

    spi_write_byte(CMD_DISP_OFF);
    
    spi_write_byte(CMD_CLK);
    spi_write_byte(0x80); // TODO: tweak value, fix magic num
    // high nibble = oscillator frequency, low nibble = divider ratio.

    spi_write_byte(CMD_MUX_RATIO);
    spi_write_byte(DISP_MUX_RATIO);

    spi_write_byte(CMD_DISP_OFFSET);
    spi_write_byte(DISP_OFFSET); 
    
    spi_write_byte(CMD_START_LINE);

    spi_write_byte(CMD_ADDR_MODE);
    spi_write_byte(DISP_ADDR_MODE_H);

    spi_write_byte(CMD_COL_ADDRS);
    spi_write_byte(DISP_COL_START);
    spi_write_byte(DISP_COL_END);

    spi_write_byte(CMD_PAGE_ADDRS);
    spi_write_byte(DISP_PAGE_START);
    spi_write_byte(DISP_PAGE_END);

    spi_write_byte(CMD_UNFLIP_H);

    spi_write_byte(CMD_UNFLIP_V);

    spi_write_byte(CMD_COM_PIN_CONF);
    spi_write_byte(DISP_COM_PIN_CONF);

    spi_write_byte(CMD_CONTRAST);
    spi_write_byte(DISP_DEFAULT_CONTRAST);

    spi_write_byte(CMD_PRECHRG_PERIOD);
    spi_write_byte(DISP_PRECHRG_PERIOD); // TODO: tweak value
    // upper nibble = charge (phase 2), lower nibble = discharge (phase 1)

    spi_write_byte(CMD_VCOMH_DESEL_LVL);
    spi_write_byte(0x20); // TODO: tweak value, fix magic num

    spi_write_byte(CMD_ALL_PX_OFF);

    spi_write_byte(CMD_UNINVERT);

    spi_write_byte(CMD_DISP_ON);
}

void clear_framebuf() {
    // D/C high (write to display RAM)
    gpio_put(DISP_DC_GPIO, true);

    for (int i = 0; i < 1024; i++) {
        framebuf[i] = 0;
        spi_write_byte(0);
    }
}

void write_framebuf() {

    // D/C high (write to display RAM)
    gpio_put(DISP_DC_GPIO, true);

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