#include "rle.h"

#define RUN_COLOR_MASK 0x8000 // clears data bits (bitwise AND)
#define RUN_COUNT_MASK 0x7FFF // clears color bit (bitwise AND)

extern uint8_t framebuf[1024];

void decompress_image(const uint16_t img[]) {
    uint16_t run = 0;
    uint16_t fb_pos = 0;

    while (img[run] != 0xFFFF) {
        uint16_t count = img[run] & RUN_COUNT_MASK;
        uint16_t color = img[run] & RUN_COLOR_MASK; // HACK: color stored as uint16, should be uint8, but
                                                    // throws overflow warning using 16bit img[run] value.
        for (uint16_t i = 0; i < (count >> 3); i++) {
            framebuf[fb_pos + i] ^= (color) ? (255) : (0);
            fb_pos++;
        }

        uint8_t last_out = 0;

        for (uint8_t i = 0; i < (count - (count >> 3)); count++) {
            last_out |= (1 << (7 - i));
        }
        framebuf[fb_pos++] ^= last_out;
        run++;
    }
}