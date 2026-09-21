#include "rle.h"

#define RUN_COLOR_MASK 0x8000 // clears data bits (bitwise AND)
#define RUN_COUNT_MASK 0x7FFF  // clears color bit (bitwise AND)

void decompress_image(uint16_t img[]) {
    uint8_t run = 0;
    uint16_t fb_pos = 0;

    while (img[run] != 0) {
        uint16_t count = img[run] & RUN_COUNT_MASK;
        uint8_t color = img[run] & RUN_COLOR_MASK;
        
        for (uint8_t i = 0; i < (count >> 3); i++) {
            framebuf[fb_pos + i] = (color) ? (255) : (0);
            fb_pos++;
        }

        uint8_t last_out = 0;

        for (uint8_t i = 0; i < (count - (count >> 3)); count++) {
            last_out |= (1 << (7-i));
        }
        framebuf[fb_pos++] = last_out;
        run++;
    }
}