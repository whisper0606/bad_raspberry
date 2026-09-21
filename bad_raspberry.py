import cv2 
from PIL import Image
import os 
# IMPORTS

# cv2 for extracting frames from video
# pillow for image manipulation
# os for filesystem manipulation


# PLAN

# Get frames from video as PNGs
# Convert frames to 1bpp monochrome bitmap at 128x64
# Iterate over bitmap to produce RLE compressed image
# Write final array to file 

# Frame Collection
cap = cv2.VideoCapture("./bad_apple.mp4")

try:
    if not os.path.exists("./frames"):
        print("Frames directory does not exist, creating...")
        os.makedirs("./frames")
        os.makedirs("./frames/raw")
    else:
        print("Frames directory already exists, not creating.")

except OSError:
    print("Failed to create frames directory, exiting...")
    exit()

current_frame = 0
print("Capturing frames...")
while True:
    ret,capframe = cap.read()
    if ret:
        cv2.imwrite(f"./frames/raw/{current_frame}.png",capframe)
        current_frame += 1
    else:
        print(f"All {current_frame} frames captured.")
        break

cap.release()
cv2.destroyAllWindows()

# Format Conversion
print("Converting frames...")
current_frame = 0
for frame in os.scandir("./frames/raw"):
    if frame.is_file():
        with Image.open(frame.path) as img:
            img = img.convert(mode = "1", dither = None)
            img = img.resize((114,64), Image.LANCZOS)
            out = Image.new(img.mode, (128,64))
            out.paste(img, (7,0))
            out.save(f"./frames/{current_frame}.bmp")
        os.remove(frame.path)
        current_frame += 1
print(f"All {current_frame} frames converted.")
os.removedirs("./frames/raw")

# Compression
compressed_imgs = [] # MASTER ARRAY

for frame in os.scandir("./frames"):
    if frame.is_file():
        with Image.open(frame.path) as img:
            img_size_x, img_size_y = img.size
            img_arr = []
            prev_px = None
            run_len = 0
            for i in range(0, img_size_x):
                for j in range(0,img_size_y):
                    current_px = img.getpixel((i,j))
                    if (i == img_size_x-1 and j == img_size_y-1) or (current_px != prev_px and prev_px != None):
                        
                    # prev_px

            # for each pixel in image:
                # if last pixel or (current_color not prev_color and prev_color not None):
                    # if current_color = white:
                        img.add(32768 + run_len)
                    else:
                        img.add(run_len)


"""
# Write to File
with open("frames.c", "w") as f:
    f.write("#include <stdint.h>")



# FINAL .c FILE FORMAT:

# 1. Define image arrays containing RLE runs (uint16_t, msb color, else count).
#     e.g. uint16_t img0[run_count] = {run_0, ... run_n, 0}; (run must have count > 0)

# 2. Define master array containing pointers to image arrays
#     e.g. uint16_t *master = {img0, img1,}
"""