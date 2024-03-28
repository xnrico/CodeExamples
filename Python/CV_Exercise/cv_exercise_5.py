"""
Task6 Code
"""
import numpy as np
import common 
from common import save_img, read_img
from homography import fit_homography, homography_transform, RANSAC_fit_homography
import os
import cv2

def compute_distance(desc1, desc2):
    '''
    Calculates L2 distance between 2 binary descriptor vectors.
        
    Input - desc1: Descriptor vector of shape (N,F)
            desc2: Descriptor vector of shape (M,F)
    
    Output - dist: a (N,M) L2 distance matrix where dist(i,j)
             is the squared Euclidean distance between row i of 
             desc1 and desc2. You may want to use the distance
             calculation trick
             ||x - y||^2 = ||x||^2 + ||y||^2 - 2x^T y
    '''
    
    desc1_norms = np.sum(desc1**2, axis=1, keepdims=True)
    desc2_norms = np.sum(desc2**2, axis=1, keepdims=True)
    
    dist = desc1_norms + desc2_norms.T - 2 * np.matmul(desc1, desc2.T)
    
    return dist

def find_matches(desc1, desc2, ratioThreshold):
    '''
    Calculates the matches between the two sets of keypoint
    descriptors based on distance and ratio test.
    
    Input - desc1: Descriptor vector of shape (N,F)
            desc2: Descriptor vector of shape (M,F)
            ratioThreshhold : maximum acceptable distance ratio between 2
                              nearest matches 
    
    Output - matches: a list of indices (i,j) 1 <= i <= N, 1 <= j <= M giving
             the matches between desc1 and desc2.
             
             This should be of size (K,2) where K is the number of 
             matches and the row [ii,jj] should appear if desc1[ii,:] and 
             desc2[jj,:] match.
    '''
    
    N = np.size(desc1, axis=0)
    M = np.size(desc2, axis=0)
    dist = compute_distance(desc1, desc2) # dist matrix (N x M)
    
    # Find the two nearest neighbors for each descriptor in desc1
    nearest = np.argpartition(dist, kth=1, axis=1)[:, :2]
    
    ratios = dist[np.arange(len(nearest)), nearest[:, 0]] / dist[np.arange(len(nearest)), nearest[:, 1]]
    mask = ratios < ratioThreshold
    matches = np.column_stack((np.where(mask)[0], nearest[mask][:, 0]))

    return matches

def draw_matches(img1, img2, kp1, kp2, matches):
    '''
    Creates an output image where the two source images stacked vertically
    connecting matching keypoints with a line. 
        
    Input - img1: Input image 1 of shape (H1,W1,3)
            img2: Input image 2 of shape (H2,W2,3)
            kp1: Keypoint matrix for image 1 of shape (N,4)
            kp2: Keypoint matrix for image 2 of shape (M,4)
            matches: List of matching pairs indices between the 2 sets of 
                     keypoints (K,2)
    
    Output - Image where 2 input images stacked vertically with lines joining 
             the matched keypoints
    '''

    pts = common.get_match_points(kp1, kp2, matches)

    # Create output image
    h1, w1 = img1.shape[:2]
    h2, w2 = img2.shape[:2]
    output = np.zeros((h1+h2, max(w1,w2), 3), dtype=np.uint8)
    output[:h1, :w1] = img1
    output[h1:h1+h2, :w2] = img2

    # Draw lines connecting matching keypoints
    for pt in (pts):
        x1, y1 = int(pt[0]), int(pt[1])
        x2, y2 = int(pt[2]), int(pt[3])
        y2 += h1
        color = tuple(np.random.randint(0, 255, 3).tolist())
        cv2.line(output, (x1, y1), (x2, y2), color, thickness=1)

    return output

def warp_and_combine(img1, img2, H):
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

    V = (warped1.astype("uint16") * img_mask1 + warped2.astype("uint16") * img_mask2) / img_mask_avg.astype("uint16")
    V = np.uint8(V)
    
    return V


def make_warped(img1, img2):
    '''
    Take two images and return an image, putting together the full pipeline.
    You should return an image of the panorama put together.
    
    Input - img1: Input image 1 of shape (H1,W1,3)
            img2: Input image 1 of shape (H2,W2,3)
    
    Output - Final stitched image
    Be careful about:
    a) The final image size 
    b) Writing code so that you first estimate H and then merge images with H.
    The system can fail to work due to either failing to find the homography or
    failing to merge things correctly.
    '''
    
    kp1, desc1 = common.get_AKAZE(I1)
    kp2, desc2 = common.get_AKAZE(I2)
    
    matches = find_matches(desc1, desc2, 0.75)
    pts = common.get_match_points(kp1, kp2, matches)
    H = RANSAC_fit_homography(pts)
    
    stitched = warp_and_combine(img1, img2, H)
    
    return stitched 


if __name__ == "__main__":
    to_stitch = 'lowetag'
    I1 = read_img(os.path.join('task6',to_stitch,'p1.jpg'))
    I2 = read_img(os.path.join('task6',to_stitch,'p3.jpg'))
    
    
    kp1, desc1 = common.get_AKAZE(I1)
    kp2, desc2 = common.get_AKAZE(I2)
    
    matches = find_matches(desc1, desc2, 0.70)    
    out = draw_matches(I1, I2, kp1, kp2, matches)
    
    res = make_warped(I1,I2)
    save_img(res,"result_"+to_stitch+".jpg")
    
    cv2.imshow("Stitched", res)
    cv2.waitKey(0)
    cv2.destroyAllWindows()
