import os
import numpy as np
from helper_functions import read_img, save_img
import scipy as sp


def image_patches(image, patch_size=(16, 16)):
    H = np.size(image, axis=0)
    W = np.size(image, axis=1)
    M = np.int_(H / (patch_size[0]))
    N = np.int_(W / (patch_size[1]))

    output = []
    for i in range(M):
        for j in range(N):
            patch = image[16*i:16*i+16, 16*j:16*j+16]
            patch = patch - np.mean(patch)
            patch = patch / np.std(patch)
            output.append(patch)
    
    return output


def convolve(image, kernel):
    kernel = np.fliplr(np.flipud(kernel))

    H = np.size(image, axis=0)
    W = np.size(image, axis=1)
    h = np.size(kernel, axis=0)
    w = np.size(kernel, axis=1)
  
    output = np.zeros(image.shape)
    IMAGE = image.copy()

    padsH = int((w - 1) / 2) # pads to be added horizontally (cols to the left & right)
    padsW = int((h - 1) / 2) # pads to be added vertically (rows to the top & bottom)

    if (padsH > 0): # add cols
        pad1 = np.zeros([H, padsH])
        IMAGE = np.hstack((pad1, IMAGE))
        IMAGE = np.hstack((IMAGE, pad1))

    if (padsW > 0): # add rows
        pad2 = np.zeros([padsW, W + 2*padsH])
        IMAGE = np.vstack((pad2, IMAGE))
        IMAGE = np.vstack((IMAGE, pad2))

    for I in range(H):
        for J in range(W):
            for i in range(h):
                for j in range(w):
                    output[I, J] += kernel[i, j] * IMAGE[I+i, J+j]

    return output


def edge_detection(image):
    kx = np.matrix('1, 0, -1')  # 1 x 3
    ky = np.matrix('1; 0; -1')  # 3 x 1

    Ix = convolve(image, kx)
    Iy = convolve(image, ky)

    # Ix = sp.ndimage.convolve(image, kx, mode='constant')
    # Iy = sp.ndimage.convolve(image, ky, mode='constant')
    grad_magnitude = (Ix ** 2 + Iy **2) ** (0.50)

    return Ix, Iy, grad_magnitude


def sobel_operator(image):
    """
    Return Gx, Gy, and the gradient magnitude.

    Input- image: H x W
    Output- Gx, Gy, grad_magnitude: H x W
    """
    Gx, Gy, grad_magnitude = None, None, None

    kx = np.matrix('1, 0, -1')  # 1 x 3
    ky = np.matrix('1; 0; -1')  # 3 x 1
    Gs = np.matrix('1, 2, 1; 2, 4, 2; 1, 2, 1')

    Sx = 1/2 * convolve(Gs, kx)
    Sy = 1/2 * convolve(Gs, ky)

    # Sx = sp.ndimage.convolve(Gs, kx, mode='constant')
    # Sy = sp.ndimage.convolve(Gs, ky, mode='constant')

    Gx = convolve(image, Sx)
    Gy = convolve(image, Sy)

    grad_magnitude = (Gx ** 2 + Gy **2) ** (0.50)

    return Gx, Gy, grad_magnitude




def main():
    # The main function
    img = read_img('./grace_hopper.png')
    """ Image Patches """
    if not os.path.exists("./image_patches"):
        os.makedirs("./image_patches")
    patches = image_patches(img)

    chosen_patches = np.array(patches[2])
    save_img(chosen_patches, "./image_patches/q1_patch.png")


    """ Convolution and Gaussian Filter """
    if not os.path.exists("./gaussian_filter"):
        os.makedirs("./gaussian_filter")

    kernel_gaussian = np.matrix("0.02289, 0.10552, 0.02289; 0.10552, 0.48644, 0.10552; 0.02289, 0.10552, 0.02289")

    filtered_gaussian = convolve(img, kernel_gaussian)

    
    save_img(filtered_gaussian, "./gaussian_filter/q2_gaussian.png")

    _, _, edge_detect = edge_detection(img)
    save_img(edge_detect, "./gaussian_filter/q3_edge.png")
    _, _, edge_with_gaussian = edge_detection(filtered_gaussian)
    save_img(edge_with_gaussian, "./gaussian_filter/q3_edge_gaussian.png")

    # exU = np.mean(edge_detect)
    # exM = np.max(edge_detect)
    # egU = np.mean(edge_with_gaussian)
    # egM = np.max(edge_with_gaussian)

    print("Gaussian Filter is done. ")

    # -- TODO Task 3: Sobel Operator --
    if not os.path.exists("./sobel_operator"):
        os.makedirs("./sobel_operator")

    Gx, Gy, edge_sobel = sobel_operator(img)
    save_img(Gx, "./sobel_operator/q2_Gx.png")
    save_img(Gy, "./sobel_operator/q2_Gy.png")
    save_img(edge_sobel, "./sobel_operator/q2_edge_sobel.png")

    print("Sobel Operator is done. ")

    # -- TODO Task 4: LoG Filter --
    if not os.path.exists("./log_filter"):
        os.makedirs("./log_filter")

    # (a)
    kernel_LoG1 = np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]])
    kernel_LoG2 = np.array([[0, 0, 3, 2, 2, 2, 3, 0, 0],
                            [0, 2, 3, 5, 5, 5, 3, 2, 0],
                            [3, 3, 5, 3, 0, 3, 5, 3, 3],
                            [2, 5, 3, -12, -23, -12, 3, 5, 2],
                            [2, 5, 0, -23, -40, -23, 0, 5, 2],
                            [2, 5, 3, -12, -23, -12, 3, 5, 2],
                            [3, 3, 5, 3, 0, 3, 5, 3, 3],
                            [0, 2, 3, 5, 5, 5, 3, 2, 0],
                            [0, 0, 3, 2, 2, 2, 3, 0, 0]])
    filtered_LoG1 = convolve(img, kernel_LoG1)
    filtered_LoG2 = convolve(img, kernel_LoG2)

    save_img(filtered_LoG1, "./log_filter/q1_LoG1.png")
    save_img(filtered_LoG2, "./log_filter/q1_LoG2.png")

    print("LoG Filter is done. ")

if __name__ == "__main__":
    main()
