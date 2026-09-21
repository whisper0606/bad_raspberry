#ifndef RLE_H
#define RLE_H

#include <stdint.h>

// IMAGE FORMAT: uint16_t ARRAY.
// RLE RUN FORMAT: uint16_t. MSB = COLOUR BIT. ALL OTHER BITS INDICATE RUN LENGTH. 

extern uint8_t framebuf[1024];

uint8_t prebuf[1024];

/* Load an RLE compressed image into the framebuffer. */
void decompress_image(uint16_t img[]);

#endif