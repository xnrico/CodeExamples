import os

import numpy as np
import scipy.ndimage as spi
import matplotlib.pyplot as plt
import cv2

from helper_functions import read_img, save_img


def corner_score(image, u=5, v=5, window_size=(5, 5)):
    I = image.copy()

    pad = int(window_size[0] / 2)
    I = np.pad(I, pad, mode='constant')

    offset_I = np.roll(np.roll(I, u, axis=0), v, axis=1)
    sq_diff = (I - offset_I)**2

    # Convolution
    window = np.ones(window_size)
    output = spi.convolve(sq_diff, window, mode='constant')
    
    return output


def harris_detector(image, window_size=(5, 5)):
    G_kernel = cv2.getGaussianKernel(ksize=5, sigma= 5/6)
    G_kernel = G_kernel @ G_kernel.T

    G = G_kernel.copy()
    I = image.copy()

    # compute the derivatives
    Ix = spi.convolve(I, np.matrix('1, 0, -1'), mode='constant')
    Iy = spi.convolve(I, np.matrix('1, 0, -1').T, mode='constant')

    Ixx = spi.convolve(Ix, np.matrix('1, 0, -1'), mode='constant')
    Iyy = spi.convolve(Iy, np.matrix('1, 0, -1').T, mode='constant')
    Ixy = spi.convolve(Iy, np.matrix('1, 0, -1'), mode='constant')

    Gxx = spi.convolve(Ixx, G_kernel, mode='constant')
    Gxy = spi.convolve(Ixy, G_kernel, mode='constant')
    Gyy = spi.convolve(Iyy, G_kernel, mode='constant')

    detM = Gxx * Gyy - Gxy * Gxy
    trM = Gxx + Gyy
    alpha = 0.04
    response = detM - alpha * trM * trM

    # For each image location, construct the structure tensor and calculate the Harris response
   
    return response


def main():
    img = read_img('./grace_hopper.png')

    # Feature Detection
    if not os.path.exists("./feature_detection"):
        os.makedirs("./feature_detection")

    u, v, W = 1, 1, (5,5)

    score = corner_score(img, u, v, W)
    save_img(score, "./feature_detection/corner_score.png")

    # Computing the corner scores for various u, v values.
    score = corner_score(img, 0, 5, W)
    save_img(score, "./feature_detection/corner_score05.png")

    score = corner_score(img, 0, -5, W)
    save_img(score, "./feature_detection/corner_score0-5.png")

    score = corner_score(img, 5, 0, W)
    save_img(score, "./feature_detection/corner_score50.png")

    score = corner_score(img, -5, 0, W)
    save_img(score, "./feature_detection/corner_score-50.png")

    harris_corners = harris_detector(img)
    fig = plt.imshow(harris_corners, cmap='hot', interpolation='nearest')
    plt.show()
    save_img(harris_corners, "./feature_detection/harris_response.png")


if __name__ == "__main__":
    main()
