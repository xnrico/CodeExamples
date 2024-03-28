import numpy as np
import cvxpy as cp

def Setup_Derivative(param):
    return None

def Derivative(x_bar, u_bar, param, control=False):
    # Extract parameters
    h = param["h"]  # Time step
    L_f = param["L_f"]  # Distance from the center of gravity to the front axle
    L_r = param["L_r"]  # Distance from the center of gravity to the rear axle

    dt = h  # Time step
    psi = x_bar[2]  # Yaw angle
    v = x_bar[3]  # Velocity
    delta = u_bar[1]  # Steering angle
    beta = np.arctan((L_r * np.arctan(delta)) / (L_f + L_r))  # Sideslip angle

    # Initialize matrices
    A = np.zeros([4, 4])  # State matrix
    B = np.zeros([4, 2])  # Control matrix
    result = []

    if control == False:
        # Compute state matrix A
        A = np.array([
            [0, 1, -dt * v * np.sin(psi + beta), dt * v * np.cos(psi + beta)],
            [0, 1, dt * v * np.cos(psi + beta), dt * v * np.sin(psi + beta)],
            [0, 0, 1, (dt * np.arctan(delta)) / (((L_r**2 * np.arctan(delta)**2) / (L_f + L_r)**2 + 1)**(1/2) * (L_f + L_r))],
            [0, 0, 0, 1]
        ])
        result = A
    else:
        # Compute control matrix B
        B = np.array([
            [0, -(dt * L_r * v * np.sin(psi + beta)) / ((delta**2 + 1) * ((L_r**2 * np.arctan(delta)**2) / (L_f + L_r)**2 + 1) * (L_f + L_r))],
            [0, (dt * L_r * v * np.cos(psi + beta)) / ((delta**2 + 1) * ((L_r**2 * np.arctan(delta)**2) / (L_f + L_r)**2 + 1) * (L_f + L_r))],
            [0, (dt * v) / ((delta**2 + 1) * ((L_r**2 * np.arctan(delta)**2) / (L_f + L_r)**2 + 1)**(3/2) * (L_f + L_r))],
            [dt, 0]
        ])
        result = B

    return result

def Student_Controller_LQR(x_bar, u_bar, x0, Fun_Jac_dt, param):
    dim_ctrl = u_bar.shape[1]
    dim_state = x_bar.shape[1]
    N = len(x_bar) - 1  # Preview horizon

    p = 100
    q = 1
    r = 20

    P = np.eye(dim_state) * p
    Q = np.eye(dim_state) * q
    R = np.eye(dim_ctrl) * r

    # Initialize variables
    delta_s = []  # State perturbations
    delta_u = []  # Control perturbations

    for k in range(N):
        delta_s.append(cp.Variable(4))
        delta_u.append(cp.Variable(2))
    delta_s.append(cp.Variable(4))

    # Define the cost function
    cost = 0
    for k in range(N):
        cost += cp.quad_form(delta_s[k], Q) + cp.quad_form(delta_u[k], R)
    cost += cp.quad_form(delta_s[N], P)  # Terminal cost

    # Define the constraints
    constraints = []
    for k in range(N):
        A_k = Derivative(x_bar[k], u_bar[k], param, control=False)  # Compute A_k matrix
        B_k = Derivative(x_bar[k], u_bar[k], param, control=True)  # Compute B_k matrix

        # State dynamics constraint
        state_constraint = delta_s[k + 1] == A_k @ delta_s[k] + B_k @ delta_u[k]
        constraints.append(state_constraint)

    # Initial state constraint
    constraints.append(delta_s[0] == x0 - x_bar[0])

    # Create the optimization problem
    prob = cp.Problem(cp.Minimize(cost), constraints)

    # Solve the problem
    prob.solve()

    delta_u_opt = [u_var.value for u_var in delta_u]
    u_act = u_bar[0] + np.array(delta_u_opt[0])

    return u_act

def Student_Controller_CMPC(x_bar, u_bar, x0, Fun_Jac_dt, param):
    dim_ctrl = u_bar.shape[1]
    dim_state = x_bar.shape[1]
    N = len(x_bar) - 1  # Preview horizon

    a_lim_lower = param["a_lim"][0]
    a_lim_upper = param["a_lim"][1]
    delta_lim_lower = param["delta_lim"][0]
    delta_lim_upper = param["delta_lim"][1]
    u_min = np.array([a_lim_lower, delta_lim_lower])
    u_max = np.array([a_lim_upper, delta_lim_upper]) # MPC Inequality Parameters

    p = 10000
    q = 500
    r = 50

    P = np.eye(dim_state) * p
    Q = np.eye(dim_state) * q
    R = np.eye(dim_ctrl) * r

    # Initialize variables
    delta_s = []  # State perturbations
    delta_u = []  # Control perturbations

    for k in range(N):
        delta_s.append(cp.Variable(4))
        delta_u.append(cp.Variable(2))
    delta_s.append(cp.Variable(4))

    # Define the cost function
    cost = 0
    for k in range(N):
        cost += cp.quad_form(delta_s[k], Q) + cp.quad_form(delta_u[k], R)
    cost += cp.quad_form(delta_s[N], P)  # Terminal cost

    # Define the constraints
    constraints = []
    for k in range(N):
        A_k = Derivative(x_bar[k], u_bar[k], param, control=False)  # Compute A_k matrix
        B_k = Derivative(x_bar[k], u_bar[k], param, control=True)  # Compute B_k matrix

        # State dynamics constraint
        state_constraint = delta_s[k + 1] == A_k @ delta_s[k] + B_k @ delta_u[k]
        constraints.append(state_constraint)

        # Input constraints
        input_constraint_1 = delta_u[k] <= u_max - u_bar[k]
        input_constraint_2 = -delta_u[k] <= u_bar[k] - u_min
        constraints.append(input_constraint_1)
        constraints.append(input_constraint_2)

    # Initial state constraint
    constraints.append(delta_s[0] == x0 - x_bar[0])

    # Create the optimization problem
    prob = cp.Problem(cp.Minimize(cost), constraints)

    # Solve the problem
    prob.solve()

    delta_u_opt = [u_var.value for u_var in delta_u]
    u_act = u_bar[0] + np.array(delta_u_opt[0])

    return u_act

if __name__ == '__main__':
    preview = 20
    x0 = np.array([2, 2, 3.14 * 0.5, 2])
    Jac = Setup_Derivative(param)

    x_log, u_log = Sim.SimVehicle(x_bar, u_bar, preview, x0, lambda x, u, xint: Student_Controller_CMPC(x, u, xint, Jac, param) )

    plt.figure(figsize=(5, 5))
    plt.plot(x_bar[0, 0], x_bar[0, 1], 'ko-')
    plt.plot(x_log[0, 0], x_log[0, 1], 'ro-')

    plt.plot(x_bar[:, 0], x_bar[:, 1], 'k-', label = "reference")
    plt.plot(x_log[:, 0], x_log[:, 1], 'r--', label = "vehicle")
    plt.axis('equal')
    plt.xlabel('$x$')
    plt.ylabel('$y$')
    plt.title('trajectory')
    plt.legend()
    plt.grid()
    plt.show()

    plt.figure(figsize=(20, 3))
    plt.plot(x_bar[:, 2], 'k-', label = "reference")
    plt.plot(x_log[:, 2], 'r--', label = "vehicle")
    plt.title('Orientation')
    plt.legend()
    plt.grid()
    plt.show()


    plt.figure(figsize=(20, 3))
    plt.plot(x_bar[:, 3], 'k-', label = "reference")
    plt.plot(x_log[:, 3], 'r--', label = "vehicle")
    plt.title('Velocity')
    plt.legend()
    plt.grid()
    plt.show()


    plt.figure(figsize=(20, 3))
    plt.plot(u_bar[:, 0], 'k-', label = "reference")
    plt.plot(u_log[:, 0], 'r--', label = "vehicle")
    plt.title('Acceleration')
    plt.legend()
    plt.grid()
    plt.show()


    plt.figure(figsize=(20, 3))
    plt.plot(u_bar[:, 1], 'k-', label = "reference")
    plt.plot(u_log[:, 1], 'r--', label = "vehicle")
    plt.title('Steering Angle')
    plt.legend()
    plt.grid()
    plt.show()