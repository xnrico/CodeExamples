import numpy as np
import common 
from common import save_img, read_img
from homography import homography_transform, RANSAC_fit_homography
import cv2
import os
import task6

def task7_warp_and_combine(img1, img2, H):
    '''
    You may want to write a function that merges the two images together given
    the two images and a homography: once you have the homography you do not
    need the correspondences; you just need the homography.
    Writing a function like this is entirely optional, but may reduce the chance
    of having a bug where your homography estimation and warping code have odd
    interactions.
    
    Input - img1: Input image 1 of shape (H1,W1,3)
            img2: Input image 2 of shape (H2,W2,3)
            H: homography mapping betwen them
    Output - V: stitched image of size (?,?,3); unknown since it depends on H
                but make sure in V, for pixels covered by both img1 and warped img2,
                you see only img2
    '''
    h1, w1 = img1.shape[:2]
    h2, w2 = img2.shape[:2]
    corners1 = np.float32([[0, 0], [0, h1], [w1, h1], [w1, 0]]).reshape(-1, 1, 2)
    corners2 = np.float32([[0, 0], [0, h2], [w2, h2], [w2, 0]]).reshape(-1, 1, 2)
    
    corners1_trans = cv2.perspectiveTransform(corners1, H)
    min_x, min_y = np.rint(np.min(np.concatenate([corners1_trans, corners2], axis=0), axis=(0, 1)))
    max_x, max_y = np.rint(np.max(np.concatenate([corners1_trans, corners2], axis=0), axis=(0, 1)))
    
    height = max_y - min_y
    width = max_x - min_x
    
    H2 = np.array(([1, 0, -min_x], [0, 1, -min_y], [0, 0, 1]))
    
    canvas = np.zeros((int(height), int(width), 3)).astype('uint8')
    
    warped1 = cv2.warpPerspective(img1, H2 @ H, (int(width), int(height)))
    warped2 = cv2.warpPerspective(img2, H2, (int(width), int(height)))
    
    img_mask1 = cv2.warpPerspective(np.ones_like(img1), H2 @ H, (int(width), int(height)))
    img_mask2 = cv2.warpPerspective(np.ones_like(img2), H2, (int(width), int(height)))
    img_mask_avg = (img_mask1 + img_mask2)

    V = warped1.astype("uint16") * img_mask1 + warped2.astype("uint16") * (img_mask2 - img_mask1)
    V = np.uint8(V)
    
    return V

def improve_image(scene, template, transfer):
    '''
    Detect template image in the scene image and replace it with transfer image.

    Input - scene: image (H,W,3)
            template: image (K,K,3)
            transfer: image (L,L,3)
    Output - augment: the image with 
    '''
    
    transfer = cv2.resize(transfer, [np.size(template, axis=0), np.size(template, axis=1)])
    
    kp1, descS = common.get_AKAZE(scene)
    kp2, descT = common.get_AKAZE(template)
    
    matches = task6.find_matches(descT, descS, 0.75)
    pts = common.get_match_points(kp2, kp1, matches)
    H = RANSAC_fit_homography(pts)
    
    augment = task7_warp_and_combine(transfer, scene, H)

    return augment

if __name__ == "__main__":
    to_stitch = 'myscene'
    scene = read_img(os.path.join('task7','scenes',to_stitch,'myscene.jpg'))
    template = read_img(os.path.join('task7','scenes',to_stitch,'mytemplate.png'))
    transfer = read_img(os.path.join('task7','seals','mytransfer.jpg'))
    
    res = improve_image(scene, template, transfer)
    save_img(res,"myimproved.jpg")
    
    cv2.imshow("SEAL", res)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
    
    pass
