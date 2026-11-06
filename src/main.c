#include <pico/stdlib.h>
#include <pico/time.h>

#include "ssd1309_driver.h"

#define DELAY 41667
#define FRAMES 5259

#define RUN_COLOR_MASK 0x8000
#define RUN_COUNT_MASK 0x7FFF

extern const uint16_t *master[];
volatile bool load_next = true;

bool timer_callback(struct repeating_timer *t) {
    write_framebuf();
    load_next = 1;
    return true;
}

void decode_img(const uint16_t *img) {
    uint16_t run = 0;
    uint16_t index = 0;
    uint8_t pixel = 0;
    uint8_t color = ((img[0] & RUN_COLOR_MASK) >> 15);

    while (index < 1024) {
        uint16_t count = (img[run] & RUN_COUNT_MASK);

        // Write until pixel == 0 || count == 0
        // if pixel == 0, we can move onto writing full u8s.
        // if count == 0, we're done.
        while (count && pixel) {
            framebuf[index] ^= (color << (7 - pixel));
            pixel++;
            count--;
            if (pixel > 7) {
                pixel = 0;
                index++;
                break;
            }
        }

        // If count <= 7, then we can't keep writing entire array elements.
        while (count > 7) {
            framebuf[index] ^= color ? 0xFF : 0x00;
            index++;
            count -= 8;
        }

        // Iterate for the remaining value of count.
        while (count) {
            framebuf[index] ^= (color << (7 - pixel));
            pixel++;
            count--;
        }

        run++;
        color ^= 1;
    }
}

int main() {
    uint16_t current_frame = FRAMES;
    struct repeating_timer frame_timer;

    init_ssd1309();
    clear_framebuf();

    add_repeating_timer_us(-DELAY, timer_callback, NULL, &frame_timer);

    while (1) {
        if (load_next) {
            if (++current_frame >= FRAMES) {
                current_frame = 0;
                clear_framebuf();
            }
            decode_img(master[current_frame]);
            load_next = 0;
        }
    }
}