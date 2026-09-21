#include <pico/stdlib.h>
#include <pico/time.h>

#include "display/rle.h"
#include "display/ssd1309_driver.h"

extern const uint16_t *master[];
uint16_t curr_frame = 0;
bool load_next;

bool timer_callback(__unused repeating_timer_t *t) {
    write_framebuf();
    load_next = true;
    curr_frame++;
    return 0;
}

int main() {
    init_ssd1309();
    repeating_timer_t frame_timer;
    add_repeating_timer_us(-33333, timer_callback, NULL, &frame_timer);
    while (curr_frame < 6752) {
        if (load_next) {
            decompress_image(master[curr_frame]);
            load_next = false;
        }
    }
}