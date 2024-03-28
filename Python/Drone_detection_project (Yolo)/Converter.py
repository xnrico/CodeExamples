import cv2
import os
import json
import numpy as np
from PIL import Image
import pdb
 
def bboxcoordinates(camera):
    with open("cam" + str(camera) +".json") as jsonfile:
        data = json.load(jsonfile)

    highest = {}

    for d in data:
        image_id = d["image_id"]
        id = image_id[17:len(image_id)-4]
        score = d["score"]
        bbox = d["bbox"]

        if id not in highest:
            highest[id] = (bbox,score)
    sorted_dict = {int(key): value for key, value in highest.items()}
    sorted_heighest = {str(key): value for key, value in sorted(sorted_dict.items())}
    return sorted_heighest

def convert(format1):
    format = format1
    
    xmins, ymins, widths, heights = format
    
    xmins = int(xmins)
    ymins = int(ymins)
    xmaxs = int(xmins + widths)
    ymaxs = int(ymins + heights)
    
    return xmins, ymins, xmaxs, ymaxs

def draw_boxes(detection, image):
    left, top, right, bottom = convert(detection[0])
    
    image = cv2.rectangle(image, (left, top), (right, bottom), (0,255,0), 5)
    image = cv2.putText(image, "{} [{:.2f}]".format("Drone", float(detection[1])),
        (left, top - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5,
        (0,0,255), 1)
    return image


def getImages(camera):
    cam_dir = "darknet/frames/cam" + str(camera)
    camImgs = os.listdir(cam_dir)
    camImgs = sorted(camImgs, key=lambda x: int(x.split("frame")[1].split(".jpg")[0]))

    camImagePaths = []
    for img in camImgs:
        img_path = os.path.join(cam_dir, img)
        print(img_path)
        camImagePaths.append(img_path)
    
    camList = []
    for i in range(2500):       
        image_path = camImagePaths[i]
        image = np.asarray(Image.open(image_path).convert('RGB'))
        image = image[:, :, ::-1].copy() 
        camList.append(image)

    return camList




def main():

    # cam0coords = bboxcoordinates(0)
    # cam0list = getImages(0)

    # cam1coords = bboxcoordinates(1)
    # cam1list = getImages(1)

    # cam2coords = bboxcoordinates(2)
    # cam2list = getImages(2)

    cam3coords = bboxcoordinates(3)
    cam3list = getImages(3)

    print("Got Images")
    
    # out0 = cv2.VideoWriter('cam0.avi', cv2.VideoWriter_fourcc(*'DIVX'), 29.97, (1920,1080))
    # out1 = cv2.VideoWriter('cam1.avi', cv2.VideoWriter_fourcc(*'DIVX'), 29.84, (1920,1080))
    # out2 = cv2.VideoWriter('cam2.avi', cv2.VideoWriter_fourcc(*'DIVX'), 50, (1920,1080))
    out3 = cv2.VideoWriter('cam3.avi', cv2.VideoWriter_fourcc(*'DIVX'), 25, (1920,1080))

    # for i in range(len(cam0list)):
    #     # filepath = 'data/frames/cam0/frame%d.jpg' % i
    #     if str(i) in cam0coords: out0.write(draw_boxes(cam0coords[str(i)],cam0list[i]))
    #     else: out0.write(cam0list[i])
    # out0.release()
    
    # for i in range(len(cam1list)):
    #     filepath = 'data/frames/cam1/frame%d.jpg' % i
    #     if str(i) in cam1coords: out1.write(draw_boxes(cam1coords[str(i)],cam1list[i]))
    #     else: out1.write(cam1list[i])
    # out1.release()
    
    # for i in range(len(cam2list)):
    #     filepath = 'data/frames/cam2/frame%d.jpg' % i
    #     if str(i) in cam2coords: out2.write(draw_boxes(cam2coords[str(i)],cam2list[i]))
    #     else: out2.write(cam2list[i])
    # out2.release()
    
    for i in range(len(cam3list)):
        filepath = 'data/frames/cam3/frame%d.jpg' % i
        if str(i) in cam3coords: out3.write(draw_boxes(cam3coords[str(i)],cam3list[i]))
        else: out3.write(cam3list[i])
    out3.release()

if __name__ == "__main__":
    main()