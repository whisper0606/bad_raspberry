#ifndef RLE_H
#define RLE_H

#include <stdint.h>

// IMAGE FORMAT: uint16_t ARRAY.
// RLE RUN FORMAT: uint16_t. MSB = COLOUR BIT. ALL OTHER BITS INDICATE RUN LENGTH. 

/* Load an RLE compressed image into the framebuffer. */
void decompress_image(const uint16_t img[]);

#endif