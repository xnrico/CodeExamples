import os

import numpy as np
import scipy.ndimage

import cv2

# Use scipy.ndimage.convolve() for convolution.
# Use same padding (mode = 'reflect'). Refer docs for further info.

from helper_functions import (find_maxima, read_img, visualize_maxima,
                    visualize_scale_space)


def gaussian_filter(image, sigma):
    """
    Input
      image: image of size HxW
      sigma: scalar standard deviation of Gaussian Kernel

    Output
      Gaussian filtered image of size HxW
    """
    H, W = image.shape

    # -- good heuristic way of setting kernel size
    kernel_size = int(2 * np.ceil(2 * sigma) + 1)
    # Ensure that the kernel size isn't too big and is odd
    kernel_size = min(kernel_size, min(H, W) // 2)
    if kernel_size % 2 == 0:
        kernel_size = kernel_size + 1

    # Similar to Corner detection, use scipy's convolution function.
    # Again, be consistent with the settings (mode = 'reflect').

    kernel = np.zeros((kernel_size, kernel_size))

    for i in range(kernel_size):
        for j in range(kernel_size):
            x = i - kernel_size // 2
            y = j - kernel_size // 2
            kernel[i, j] = (1 / (2*np.pi*sigma**2) * np.exp(-(x**2 + y**2) / (2*sigma**2)))

    output = scipy.ndimage.convolve(image, kernel, mode='reflect')
    return output

def main():
    image = read_img('polka.png')
    # Create directory for polka_detections
    if not os.path.exists("./polka_detections"):
        os.makedirs("./polka_detections")

    # (a), (b): Detecting Polka Dots
    # First, complete gaussian_filter()
    print("Detecting small polka dots")
    # -- Detect Small Circles
    k = 1.10
    sigma_1 = 3.25
    sigma_2 = k * sigma_1
    gauss_1 = gaussian_filter(image, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image, sigma_2)  # to implement

    # calculate difference of gaussians
    DoG_small = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(DoG_small, k_xy=10)
    visualize_scale_space(DoG_small, sigma_1, sigma_2 / sigma_1,
                          './polka_detections/polka_small_DoG.png')
    visualize_maxima(image, maxima, sigma_1, sigma_2 / sigma_1,
                     './polka_detections/polka_small.png')

    # -- Detect Large Circles
    print("Detecting large polka dots")
    k = 1.2
    sigma_1 = 7.75
    sigma_2 = k * sigma_1
    gauss_1 = gaussian_filter(image, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image, sigma_2)  # to implement

    # calculate difference of gaussians
    DoG_large = gauss_2 - gauss_1  # to implement

    # visualize maxima
    # Value of k_xy is a sugguestion; feel free to change it as you wish.
    maxima = find_maxima(DoG_large, k_xy=10)
    visualize_scale_space(DoG_large, sigma_1, sigma_2 / sigma_1,
                          './polka_detections/polka_large_DoG.png')
    visualize_maxima(image, maxima, sigma_1, sigma_2 / sigma_1,
                     './polka_detections/polka_large.png')

    print("Detecting cells")

    # Detect the cells in any four (or more) images from vgg_cells
    # Create directory for cell_detections
    if not os.path.exists("./cell_detections"):
        os.makedirs("./cell_detections")

    image004 = read_img('./cells/004cell.png')
    image030 = read_img('./cells/030cell.png')
    image048 = read_img('./cells/048cell.png')
    image056 = read_img('./cells/056cell.png')
    image061 = read_img('./cells/061cell.png')
    
    sharpen_kernel = np.matrix('0, 0, 0, 0, 0; 0, 0, 0, 0, 0; 0, 0, 2.0, 0, 0; 0, 0, 0, 0, 0; 0, 0, 0, 0, 0')
    sharpen_kernel -= 1/25 * np.ones((5, 5)).astype(np.float64)

    k = 1.05
    sigma_1 = 5
    sigma_2 = k * sigma_1

    # IMG004
    gauss_1 = gaussian_filter(image004, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image004, sigma_2)  # to implement

    gauss_1 = scipy.ndimage.convolve(gauss_1, sharpen_kernel)
    gauss_2 = scipy.ndimage.convolve(gauss_2, sharpen_kernel)    

    # calculate difference of gaussians
    I004 = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(I004, k_xy=10)
    visualize_scale_space(I004, sigma_1, sigma_2 / sigma_1,
                          './cell_detections/004_DoG.png')
    visualize_maxima(image004, maxima, sigma_1, sigma_2 / sigma_1,
                     './cell_detections/004.png')

    # IMG030
    gauss_1 = gaussian_filter(image030, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image030, sigma_2)  # to implement

    gauss_1 = scipy.ndimage.convolve(gauss_1, sharpen_kernel)
    gauss_2 = scipy.ndimage.convolve(gauss_2, sharpen_kernel)    

    # calculate difference of gaussians
    I030 = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(I030, k_xy=10)
    visualize_scale_space(I030, sigma_1, sigma_2 / sigma_1,
                          './cell_detections/030_DoG.png')
    visualize_maxima(image030, maxima, sigma_1, sigma_2 / sigma_1,
                     './cell_detections/030.png')

    # IMG048
    gauss_1 = gaussian_filter(image048, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image048, sigma_2)  # to implement

    gauss_1 = scipy.ndimage.convolve(gauss_1, sharpen_kernel)
    gauss_2 = scipy.ndimage.convolve(gauss_2, sharpen_kernel)    

    # calculate difference of gaussians
    I048 = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(I048, k_xy=10)
    visualize_scale_space(I048, sigma_1, sigma_2 / sigma_1,
                          './cell_detections/048_DoG.png')
    visualize_maxima(image048, maxima, sigma_1, sigma_2 / sigma_1,
                     './cell_detections/048.png')

# IMG056
    gauss_1 = gaussian_filter(image056, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image056, sigma_2)  # to implement

    gauss_1 = scipy.ndimage.convolve(gauss_1, sharpen_kernel)
    gauss_2 = scipy.ndimage.convolve(gauss_2, sharpen_kernel)    

    # calculate difference of gaussians
    I056 = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(I056, k_xy=10)
    visualize_scale_space(I056, sigma_1, sigma_2 / sigma_1,
                          './cell_detections/056_DoG.png')
    visualize_maxima(image056, maxima, sigma_1, sigma_2 / sigma_1,
                     './cell_detections/056.png')

# IMG061
    gauss_1 = gaussian_filter(image061, sigma_1)  # to implement
    gauss_2 = gaussian_filter(image061, sigma_2)  # to implement

    gauss_1 = scipy.ndimage.convolve(gauss_1, sharpen_kernel)
    gauss_2 = scipy.ndimage.convolve(gauss_2, sharpen_kernel)    

    # calculate difference of gaussians
    I061 = gauss_2 - gauss_1  # to implement

    # visualize maxima
    maxima = find_maxima(I061, k_xy=10)
    visualize_scale_space(I061, sigma_1, sigma_2 / sigma_1,
                          './cell_detections/061_DoG.png')
    visualize_maxima(image061, maxima, sigma_1, sigma_2 / sigma_1,
                     './cell_detections/061.png')

if __name__ == '__main__':
    main()
