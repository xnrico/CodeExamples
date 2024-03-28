from utils import dehomogenize, homogenize, draw_epipolar, visualize_pcd
import numpy as np
import cv2
import pdb
import os


def find_fundamental_matrix(shape, pts1, pts2):
    """
    Computes Fundamental Matrix F that relates points in two images by the:

        [u' v' 1] F [u v 1]^T = 0
        or
        l = F [u v 1]^T  -- the epipolar line for point [u v] in image 2
        [u' v' 1] F = l'   -- the epipolar line for point [u' v'] in image 1

    Where (u,v) and (u',v') are the 2D image coordinates of the left and
    the right images respectively.

    Inputs:
    - shape: Tuple containing shape of img1
    - pts1: Numpy array of shape (N,2) giving image coordinates in img1
    - pts2: Numpy array of shape (N,2) giving image coordinates in img2

    Returns:
    - F: Numpy array of shape (3,3) giving the fundamental matrix F
    """

    FOpenCV, _ = cv2.findFundamentalMat(pts1, pts2, cv2.FM_8POINT)

    s = np.sqrt(shape[0]**2 + shape[1]**2)
    T = np.matrix([[1/s, 0, -1/2], [0, 1/s, -1/2], [0, 0, 1]])
    
    # Apply homography to input points using perspectiveTransform()
    pts1_t = cv2.perspectiveTransform(pts1.reshape(-1,1,2), T).reshape(pts1.shape[0],pts1.shape[1])
    pts2_t = cv2.perspectiveTransform(pts2.reshape(-1,1,2), T).reshape(pts2.shape[0],pts2.shape[1])

    u1 = pts1_t[:, 0]
    u2 = pts2_t[:, 0]
    v1 = pts1_t[:, 1]
    v2 = pts2_t[:, 1]

    U = np.matrix([u2*u1, u2*v1, u2, v2*u1, v2*v1, v2, u1, v1]).T
    U = np.hstack([U, np.ones([U.shape[0], 1])])

    w, F_init = np.linalg.eig(U.T @ U)

    idx = np.argmin(w)
    F_init = F_init[:, idx].reshape(3, 3)
    
    U_SVD, S, VT = np.linalg.svd(F_init)
    S1 = np.eye(3)
    S1[0][0] = S[0]
    S1[1][1] = S[1]
    S1[2][2] = 0

    F_rank2 = U_SVD @ S1 @ VT

    F = T.T @ F_rank2 @ T
    
    F = F / np.max(F)

    return F


def compute_epipoles(F):
    """
    Given a Fundamental Matrix F, return the epipoles represented in
    homogeneous coordinates.

    Check: e2@F and F@e1 should be close to [0,0,0]

    Inputs:
    - F: the fundamental matrix

    Return:
    - e1: the epipole for image 1 in homogeneous coordinates
    - e2: the epipole for image 2 in homogeneous coordinates
    """

    U, S, VT = np.linalg.svd(F)
    V = VT.T
    UT = U.T

    idx = np.argmin(S)

    null_space_left = UT[idx, :].T
    null_space_right = V[:, idx]

    e1 = null_space_right / null_space_right[2][0]
    e2 = null_space_left / null_space_left[2][0]

    return e1, e2


def find_triangulation(K1, K2, F, pts1, pts2):
    """
    Extracts 3D points from 2D points and camera matrices. Let X be a
    point in 3D in homogeneous coordinates. For two cameras, we have

        p1 === M1 X
        p2 === M2 X

    Triangulation is to solve for X given p1, p2, M1, M2.

    Inputs:
    - K1: Numpy array of shape (3,3) giving camera instrinsic matrix for img1
    - K2: Numpy array of shape (3,3) giving camera instrinsic matrix for img2
    - F: Numpy array of shape (3,3) giving the fundamental matrix F
    - pts1: Numpy array of shape (N,2) giving image coordinates in img1
    - pts2: Numpy array of shape (N,2) giving image coordinates in img2

    Returns:
    - pcd: Numpy array of shape (N,4) giving the homogeneous 3D point cloud
      data
    """
    pcd = None

    E = (K2.astype(np.float64)).T @ F @ (K1.astype(np.float64))

    R1, R2, t = cv2.decomposeEssentialMat(E)

    M1 = K1.astype(np.float64) @ np.hstack([np.eye(3), np.zeros((3,1))])
    M21 = K2.astype(np.float64) @ np.hstack([R1, t])
    M22 = K2.astype(np.float64) @ np.hstack([R1, -t])
    M23 = K2.astype(np.float64) @ np.hstack([R2, t])
    M24 = K2.astype(np.float64) @ np.hstack([R2, -t])

    out1 = cv2.triangulatePoints(M1, M21, pts1.T, pts2.T)
    sum1 = np.sum(out1[3,:] > 0)

    out2 = cv2.triangulatePoints(M1, M22, pts1.T, pts2.T)
    sum2 = np.sum(out2[3,:] > 0)

    out3 = cv2.triangulatePoints(M1, M23, pts1.T, pts2.T)
    sum3 = np.sum(out3[3,:] > 0)

    out4 = cv2.triangulatePoints(M1, M24, pts1.T, pts2.T)
    sum4 = np.sum(out4[3,:] > 0)

    idx = np.argmax([sum1, sum2, sum3, sum4])

    if (idx == 0):
        M2 = M21
    elif (idx == 1):
        M2 = M22
    elif (idx == 2):
        M2 = M23
    elif (idx == 3):
        M2 = M24

    pcd = cv2.triangulatePoints(M1, M2, pts1.T, pts2.T).T
    pcd = pcd / pcd[:, 3].reshape(-1,1)

    return pcd


if __name__ == '__main__':
    names = os.listdir("task23")
    output = "results/"

    if not os.path.exists(output):
        os.mkdir(output)

    for name in names:
        print(name)

        # load the information
        img1 = cv2.imread(os.path.join("task23", name, "im1.png"))
        img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
        img2 = cv2.imread(os.path.join("task23", name, "im2.png"))
        img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
        data = np.load(os.path.join("task23", name, "data.npz"))
        pts1 = data['pts1'].astype(float)
        pts2 = data['pts2'].astype(float)
        K1 = data['K1']
        K2 = data['K2']
        shape = img1.shape

        # compute F
        F = find_fundamental_matrix(shape, pts1, pts2)
        # compute the epipoles
        e1, e2 = compute_epipoles(F)
        print(e1, e2)
        #to get the real coordinates, divide by the last entry
        print(e1[:2]/e1[-1], e2[:2]/e2[-1])

        outname = os.path.join(output, name + "_us.png")

        draw_epipolar(img1, img2, F, pts1[::10, :], pts2[::10, :],
                      epi1=e1, epi2=e2, filename=outname)

        if 1:
            pcd = find_triangulation(K1, K2, F, pts1, pts2)
            visualize_pcd(pcd.T,
                          filename=os.path.join(output, name + "_rec.png"))


