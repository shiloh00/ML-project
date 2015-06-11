#!/usr/bin/env python


import cv2
import sys
import os
import shutil
import re

def print_usage():
    print("./dataset_draw.py dataset_path marked_path num_img")

if len(sys.argv) != 4:
    print_usage()
    sys.exit(1)

dataset_path = sys.argv[1]
marked_path = sys.argv[2]
num_img = int(sys.argv[3])

def read_image(path):
    image = cv2.imread(path)
    return image

def mark_img(img, shape):
    for (x, y) in shape:
        cv2.circle(img, (int(x), int(y)), 2, (0, 255, 0), 2)
    return img

def get_file_list(path):
    img_list = []
    pts_list = []
    for item in os.listdir(path):
        ext = item.split(".")[-1]
        if ext == "png" or ext == "jpg":
            img_name = item[:-len(ext)-1]
            img_list.append(item)
            pts_list.append(img_name+".pts")
    return (img_list, pts_list)

def read_pts(pts_path):
    shape = []
    fcontent = open(pts_path).read()
    pat = re.compile("version:\s*(\d+)[\s\r\n]*n_points:\s*(\d+)[\s\r\n]*\{[\s\r\n]*(\d[\d\s\r\n\.]+\d)[\s\r\n]*\}")
    shape_str = pat.findall(fcontent)[0][2]
    points = shape_str.split("\n")
    for point in points:
        coord = point.strip().split()
        shape.append((float(coord[0]),float(coord[1])))
    return shape

def main():
    (img_list, pts_list) = get_file_list(dataset_path)
    try:
        os.makedirs(marked_path)
    except:
        pass
    success_count = 0
    if num_img < len(img_list):
        img_list = img_list[:num_img]
    for idx in range(0, len(img_list)):
        img_path = os.path.join(dataset_path, img_list[idx])
        pts_path = os.path.join(dataset_path, pts_list[idx])
        marked_img_path = os.path.join(marked_path, str(success_count+1).zfill(4) + ".jpg")
        #print img_path, pts_path, gray_img_path, new_pts_path, marked_img_path
        img = read_image(img_path)
        shape = read_pts(pts_path)
        success_count += 1
        marked_img = mark_img(img, shape)
        cv2.imwrite(marked_img_path, marked_img)
        if (idx + 1) % 100 == 0:
            print "finish", (idx + 1), "images", "success =", success_count
    print "total succes =>", success_count

if __name__ == "__main__":
    main()
