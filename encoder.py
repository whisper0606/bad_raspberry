import cv2 
from PIL import Image
import os 
import numpy as np

# Frame capture
cap = cv2.VideoCapture("./bad_apple_24fps.mp4")

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

# Format conversion
print("Converting frames...")

current_frame = 0

for frame in sorted(os.scandir("./frames/raw"),key=lambda n : int(n.name.replace(".png",""))):
    if frame.is_file():
        with Image.open(frame.path) as img:
            img = img.convert(mode = "1", dither=Image.Dither.NONE)
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

prev_img = None
compressed_imgs = []

for frame in sorted(os.scandir("./frames"),key=lambda n : int(n.name.replace(".bmp",""))):
    if frame.is_file():
        curr_img = Image.open(frame.path)
        img = None
        
        if prev_img is None:
            prev_img = curr_img
            img = curr_img
        else:
            curr_arr = np.asarray(curr_img)
            prev_arr = np.asarray(prev_img)
            delta_arr = np.bitwise_xor(prev_arr, curr_arr)
            img = Image.fromarray(delta_arr)

        img_dat = list(img.getdata())
        rle = []
        count = 1

        if img_dat[0] == 255:
            count += 32768

        for i in range(1, len(img_dat)):
            if img_dat[i] == img_dat[i - 1]:
                count += 1
            else:
                rle.append(count)
                count = 1

        rle.append(count)
        prev_img = curr_img

        compressed_imgs.append(rle)
print("All images encoded.")

# Write to file
print("Writing frames.c...")
with open("frames.c", "w") as f:
    f.write("#include <stdint.h>\n\n")

    for frame_num, frame in enumerate(compressed_imgs):
        f.write(f"const uint16_t img{frame_num}[{len(frame)}] = \u007b") # \u007b == {
        for run_num, run in enumerate(frame):
            if run_num == len(frame) - 1:
                f.write(f"{run}\u007d;\n") # u007d == }
            else:
                f.write(f"{run}, ")
    f.write("\n")
    f.write(f"const uint16_t *master[{len(compressed_imgs)}] = \u007b")
    for i in range(len(compressed_imgs)):
        if i == len(compressed_imgs) - 1:
            f.write(f"img{i}\u007d;")
        else:
            f.write(f"img{i}, ")

print("Cleaning up...")

for f in os.scandir("./frames"):
    os.remove(f.path)
os.removedirs("./frames")

print("All done.")