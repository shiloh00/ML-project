#!/usr/bin/env python


import sys
import cv2
import os
import numpy as np

def usage():
    print("usage: ./gen_thumbnail.py input_dir output_image")

def main():
    if len(sys.argv) != 3:
        usage()
        sys.exit(0)

    input_dir = sys.argv[1]
    output = sys.argv[2]
    img_list = [os.path.join(input_dir, f) for f in os.listdir(input_dir) if os.path.isfile(os.path.join(input_dir, f))]
    width = 600
    height = 400
    single_height = 100
    sx = 0
    sy = 0
    print img_list
    img = np.zeros((height, width, 3), np.uint8)
    img[:,:] = (255, 255, 255)
    for cur_file in img_list:
        if sx >= 600:
            sx = 0
            sy += single_height
        if sy >= height:
            break
        cur_img = cv2.imread(cur_file)
        cur_height = cur_img.shape[0]
        cur_width = cur_img.shape[1]
        if cur_height > single_height:
            cur_height = single_height
        if cur_width > width - sx:
            cur_width = width - sx
        img[sy:sy+cur_height,sx:sx+cur_width] = cur_img[0:cur_height,0:cur_width]
        sx = sx + cur_width
        print cur_file

    cv2.imwrite(output, img)


if __name__ == "__main__":
    main()
