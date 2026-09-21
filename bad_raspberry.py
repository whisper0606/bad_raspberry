import cv2 
from PIL import Image
import os 
import numpy as np

# Frame Collection
cap = cv2.VideoCapture("./bad_apple.mp4")

if os.path.exists("frames.c"):
    os.remove("frames.c")

if os.path.exists("./frames/raw"):
    for f in os.scandir("./frames/raw"):
        os.remove(f.path)
    os.removedirs("./frames/raw")

if os.path.exists("./frames"):
    for f in os.scandir("./frames"):
        os.remove(f.path)
    os.removedirs("./frames")

os.makedirs("./frames")
os.makedirs("./frames/raw")

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
for frame in sorted(os.scandir("./frames/raw"),key=lambda n : int(n.name.replace(".png",""))):
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

# Encoding

print("Encoding images...")
compressed_imgs = []
prev_img = None

def add_img(leng, px):
    if px != 0:
        img_arr.append(32768+leng)
    else:
        img_arr.append(leng)

for frame in sorted(os.scandir("./frames"),key=lambda n : int(n.name.replace(".bmp",""))):
    prev_px = None
    img_arr = []
    run_len = 0
    if frame.is_file():
        img = Image.open(frame.path)
        img_x, img_y = img.size
        curr_img = np.asarray(img)
        if prev_image is not None:
            curr_img ^= prev_img
            curr_img = Image.fromarray(curr_image)
        else:
            curr_img = img 
        for y in range(img_y):
            for x in range(img_x):
                curr_px = curr_img.getpixel((x,y))
                if (curr_px != prev_px and prev_px != None):
                    if (x == img_x-1 and y == img_y-1):
                        add_img(run_len, prev_px)
                        add_img(1, curr_px)
                        run_len = 0
                    else:
                        add_img(run_len, prev_px)
                        run_len = 1
                elif (x == img_x-1 and y == img_y-1):
                    add_img(run_len+1, curr_px)
                    run_len = 0
                else:
                    run_len += 1
                prev_px = curr_px
    compressed_imgs.append(img_arr)
    prev_image = curr_image

print("All images encoded.")

# Write to file
print("Writing frames.c...")
with open("frames.c", "w") as f:
    f.write("#include <stdint.h>\n\n")

    for frame_num, frame in enumerate(compressed_imgs):
        f.write(f"const uint16_t img{frame_num}[{len(frame)+1}] = \u007b") # \u007b == {
        for run_num, run in enumerate(frame):
            if run_num == len(frame) - 1:
                f.write(f"{run}, 0xFFFF\u007d;\n") # u007d == }
            else:
                f.write(f"{run}, ")
    f.write("\n")
    f.write(f"const uint16_t *master[{len(compressed_imgs)}] = \u007b")
    for i in range(len(compressed_imgs)):
        if i == len(compressed_imgs) - 1:
            f.write(f"img{i}\u007d;")
        else:
            f.write(f"img{i}, ")

# print("Cleaning up...")

for f in os.scandir("./frames"):
    os.remove(f.path)
os.removedirs("./frames")

print("All done.")