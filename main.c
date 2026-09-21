#include <pico/stdlib.h>
#include <pico/time.h>

#include "display/ssd1309_driver.h"
#include "display/rle.h"

extern uint16_t *frames[];
bool load_next;

bool timer_callback(__unused repeating_timer_t *t) {
    write_framebuf();
    load_next = true;
    return 0;
}

int main()
{
    init_ssd1309();
    uint16_t current_frame = 0;
    repeating_timer_t frame_timer;
    // fire timer every 33.333ms, regardless of how long it took to run the callback.
    add_repeating_timer_us(-33333, timer_callback, NULL, &frame_timer);
    while (1) {
        if (load_next) {
            decompress_image(frames[current_frame]);
            load_next = false;
        }
    }
}