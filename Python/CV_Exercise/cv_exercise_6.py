"""
Task 5 Code
"""
import numpy as np
from matplotlib import pyplot as plt
from common import save_img, read_img
from homography import fit_homography, homography_transform
import os
import cv2


def make_synthetic_view(img, corners, size):
    h = size[0, 0]
    w = size[0, 1]
    
    [x1, y1] = [0, 0]
    [x2, y2] = [100*w - 1, 0]
    [x3, y3] = [100*w - 1, 100*h - 1]
    [x4, y4] = [0, 100*h - 1]
    
    XY = np.hstack((corners, np.matrix([[x1, y1], [x2, y2], [x3, y3], [x4, y4]])))
    XY = np.asarray(XY)
    print(XY)
    
    H = fit_homography(XY)
    
    output = cv2.warpPerspective(img, H, (int(100*w), int(100*h)))
        
    return output
    
if __name__ == "__main__":
    case_name = "palmer"

    I = read_img(os.path.join("task5",case_name,"book.jpg"))
    corners = np.load(os.path.join("task5",case_name,"corners.npy"))
    size = np.load(os.path.join("task5",case_name,"size.npy"))

    result = make_synthetic_view(I, corners, size)
    save_img(result, case_name+"_frontoparallel.jpg")
    
    cv2.imshow("task", result)
    cv2.waitKey(0)
    cv2.destroyAllWindows() 


