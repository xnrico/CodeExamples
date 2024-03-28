import numpy as np
import utils
from scipy.spatial.distance import cdist


def find_projection(pts2d, pts3d):
    """
    Computes camera projection matrix M that goes from world 3D coordinates
    to 2D image coordinates.

    [u v 1]^T === M [x y z 1]^T

    Where (u,v) are the 2D image coordinates and (x,y,z) are the world 3D
    coordinates

    Inputs:
    - pts2d: Numpy array of shape (N,2) giving 2D image coordinates
    - pts3d: Numpy array of shape (N,3) giving 3D world coordinates

    Returns:
    - M: Numpy array of shape (3,4)

    """
    M = None

    pts3d_norm = np.hstack((pts3d, np.ones((pts3d.shape[0], 1))))
    pts2d_norm = np.hstack((pts2d, np.ones((pts2d.shape[0], 1))))
    
    # Formulate the linear system of equations
    A = np.zeros((2 * pts2d_norm.shape[0], 12))
    for i in range(pts2d_norm.shape[0]):
        A[2*i, 0:4] = pts3d_norm[i]
        A[2*i, 8:12] = -pts2d_norm[i, 0] * pts3d_norm[i]
        A[2*i+1, 4:8] = pts3d_norm[i]
        A[2*i+1, 8:12] = -pts2d_norm[i, 1] * pts3d_norm[i]

    # Solve the linear system using SVD
    U, S, V = np.linalg.svd(A)
    M = V[-1].reshape((3, 4))
    
    return M

def compute_distance(pts2d, pts3d):
    """
    use find_projection to find matrix M, then use M to compute the average 
    distance in the image plane (i.e., pixel locations) 
    between the homogeneous points M X_i and 2D image coordinates p_i

    Inputs:
    - pts2d: Numpy array of shape (N,2) giving 2D image coordinates
    - pts3d: Numpy array of shape (N,3) giving 3D world coordinates

    Returns:
    - float: a average distance you calculated (threshold is 0.01)

    """
    distance = None

    M = find_projection(pts2d, pts3d)

    pts3d_norm = np.hstack((pts3d, np.ones((pts3d.shape[0], 1))))
    pts2d_norm = np.hstack((pts2d, np.ones((pts2d.shape[0], 1))))

    pts2d_proj = ((M @ pts3d_norm.T).T / ((M @ pts3d_norm.T).T)[:, 2].reshape(-1, 1))[:,:2]
    diff = np.sqrt(np.sum((pts2d - pts2d_proj) ** 2, axis=1))

    return np.mean(diff)

if __name__ == '__main__':
    pts2d = np.loadtxt("task1/pts2d.txt")
    pts3d = np.loadtxt("task1/pts3d.txt")

    M = find_projection(pts2d, pts3d)
    print("Matrix: ", M )
   
    foundDistance = compute_distance(pts2d, pts3d)
    print("Distance: %f" % foundDistance)
