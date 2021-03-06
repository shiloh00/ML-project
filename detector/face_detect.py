#!/usr/bin/env python


import cv2
import sys
import os
import shutil
import re

def print_usage():
    print("./face_detect.py dataset_path rect_file pts_file new_path marked_path")

if len(sys.argv) != 6:
    print_usage()
    sys.exit(1)

# Get user supplied values
dataset_path = sys.argv[1]
rect_path = sys.argv[2]
shape_path = sys.argv[3]
new_path = sys.argv[4]
marked_path = sys.argv[5]

#cascPath = sys.argv[2]
cascPath = "./haarcascade_frontalface_default.xml"

# Create the haar cascade
faceCascade = cv2.CascadeClassifier(cascPath)
# Read the image
def read_image(path):
    image = cv2.imread(path)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    return gray

# Detect faces in the image

def detect_faces(gray):
    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.2,
        minNeighbors=5,
        minSize=(30, 30),
        flags = cv2.cv.CV_HAAR_SCALE_IMAGE)
    return faces

def mark_img(img, faces, shape):
#    img = read_image(path)
#    faces = detect_faces(img)
    
#    print "Found {0} faces!".format(len(faces))
    
    for (x, y, w, h) in faces:
        cv2.rectangle(img, (x, y), (x+w, y+h), (255, 255, 255), 2)
#        print x,y,w,h

    for (x, y) in shape:
        cv2.circle(img, (int(x), int(y)), 2, (0, 255, 0), 2)
    
    return img
#    cv2.imwrite("out.jpg", img)

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
#    print pts_path
    fcontent = open(pts_path).read()
    pat = re.compile("version:\s*(\d+)[\s\r\n]*n_points:\s*(\d+)[\s\r\n]*\{[\s\r\n]*(\d[\d\s\r\n\.]+\d)[\s\r\n]*\}")
    shape_str = pat.findall(fcontent)[0][2]
    points = shape_str.split("\n")
    for point in points:
        coord = point.strip().split()
        shape.append((float(coord[0]),float(coord[1])))
    #print points, len(points)
#    print shape
#    sys.exit(0)
    return shape

def main():
    (img_list, pts_list) = get_file_list(dataset_path)
    rect_fp = open(rect_path, "w")
    shape_fp = open(shape_path, "w")
    try:
        os.makedirs(new_path)
    except:
        pass
    try:
        os.makedirs(marked_path)
    except:
        pass
    success_count = 0
    for idx in range(0, len(img_list)):
        img_path = os.path.join(dataset_path, img_list[idx])
        pts_path = os.path.join(dataset_path, pts_list[idx])
        gray_img_path = os.path.join(new_path, str(success_count+1) + ".jpg")
        new_pts_path = os.path.join(new_path, str(success_count+1) + ".pts")
        marked_img_path = os.path.join(marked_path, str(success_count+1) + ".jpg")
        #print img_path, pts_path, gray_img_path, new_pts_path, marked_img_path
        gray_img = read_image(img_path)
        faces = detect_faces(gray_img)
        shape = read_pts(pts_path)
        if len(faces) != 1:
            print "fuck?", img_path
            continue
        for (x, y, w, h) in faces:
            rect_fp.write(str(x)+" "+str(y)+" "+str(w)+" "+str(h)+"\n")
        for (x, y) in shape:
            shape_fp.write(str(x)+" ")
        for (x, y) in shape:
            shape_fp.write(str(y)+" ")
        shape_fp.write("\n")
#        print shape
        #print len(faces)
#            print "non-one faces image => ", img_list[idx], "has", len(faces), "at", idx, "success count =", success_count
#            shutil.move(img_path, os.path.join(new_path, img_list[idx]))
#            shutil.move(pts_path, os.path.join(new_path, pts_list[idx]))
#        else:
#            success_count += 1

        success_count += 1
        #cv2.imwrite(gray_img_path, gray_img)
        shutil.copyfile(img_path, gray_img_path)
        shutil.copyfile(pts_path, os.path.join(new_path, str(success_count) + ".pts"))
        marked_img = mark_img(gray_img, faces, shape)
        cv2.imwrite(marked_img_path, marked_img)
        if (idx + 1) % 100 == 0:
            print "finish", (idx + 1), "images", "success =", success_count
        #shape = read_pts(pts_path)
    print "total succes =>", success_count
    rect_fp.close()
    shape_fp.close()
    #process_path(imagePath)

if __name__ == "__main__":
    main()
