import numpy as np
from scipy.linalg import expm

def linear_ode(A):
    # Define the time step and simulation duration
    dt = 0.01  # Time step
    T = 6.0    # Total simulation time
    num_steps = int(T / dt)

    # Initial state
    x0 = np.array([1, 1])

    # Compute the matrix exponential of A
    exp_A = expm(A*dt)

    # Simulate the system over time
    state_history = [x0]
    for _ in range(num_steps):
        x_next = exp_A @ state_history[-1]
        state_history.append(x_next)

    x1_vals = [x[0] for x in state_history]
    x2_vals = [x[1] for x in state_history]

    continuous_eigenvalues, _ = np.linalg.eig(A)
    discretized_eigenvalues, _ = np.linalg.eig(exp_A)

    return continuous_eigenvalues, discretized_eigenvalues, exp_A, x1_vals, x2_vals

def sim_car():
    L = 2  # Length from front to rear axle

    # System dynamics
    def car_model(t, y, u):
        p1, p2, psi, v = y
        delta, a = u
        dp1_dt = v * np.cos(psi)
        dp2_dt = v * np.sin(psi)
        dpsi_dt = v / L * np.tan(delta) 
        dv_dt = a
        return [dp1_dt, dp2_dt, dpsi_dt, dv_dt]
    
    # Initial state
    y0 = [0, 0, 0, 1]   # [x, y, psi, v]
    # Control inputs
    u = [0.1, 0.1]     # [delta, a]

    # Integrate system dynamics
    t_span = [0, 10] # 10 seconds
    # Time points for evaluation (t_eval)
    t_eval = np.arange(0, 10, 0.01)
    sol = solve_ivp(lambda t, y: car_model(t, y, u), t_span, y0, t_eval=t_eval)

    return sol

def sim_car_control():
    import numpy as np
    from scipy.integrate import solve_ivp
    # Constants
    L = 2  # Length from front to rear axle

    # System dynamics
    def car_model(t, y, delta_func, a_func):
        p1, p2, psi, v = y
        delta = delta_func
        a = a_func
        dp1_dt = v * np.cos(psi)
        dp2_dt = v * np.sin(psi)
        dpsi_dt = v / L * np.tan(delta)
        dv_dt = a
        return [dp1_dt, dp2_dt, dpsi_dt, dv_dt]

    # Initial state
    y0 = [0, 0, 0, 1]   # [x, y, psi, v]

    # Periodic control inputs
    def periodic_delta(t):
        t = np.array(t)
        return 0.1 * np.sin(0.5 * t)

    def periodic_a(t):
        t = np.array(t)
        return 0.1 * np.cos(0.25 * t)

    # Integrate system dynamics
    t_span = [0, 20]  # 20 seconds
    # Time points for evaluation (t_eval)
    t_eval = np.arange(0, 20, 0.01)
    sol = solve_ivp(lambda t, y: car_model(t, y, periodic_delta(t), periodic_a(t)), t_span, y0, t_eval=t_eval)

    return sol

def sim_car_euler():
    L = 2  # Length from front to rear axle
    dt = 0.01  # Time step

    # System dynamics
    def car_model(y, delta, a):
        p1, p2, psi, v = y
        dp1_dt = v * np.cos(psi)
        dp2_dt = v * np.sin(psi)
        dpsi_dt = v / L * np.tan(delta)
        dv_dt = a
        return [dp1_dt, dp2_dt, dpsi_dt, dv_dt]

    # Periodic control inputs
    def periodic_delta(t):
        return 0.1 * np.sin(0.5 * np.float64(t))

    def periodic_a(t):
        return 0.1 * np.cos(0.25 * np.float64(t))

    # Initial state
    y = np.array([0, 0, 0, 1])  # [x, y, psi, v]

    # Simulation
    t_max = 20
    num_steps = int(t_max / dt)
    results = np.zeros((num_steps, 4))
    time = np.zeros(num_steps)

    for i in range(num_steps):
        delta = periodic_delta(i * dt)
        a = periodic_a(i * dt)
        results[i] = y
        time[i] = i * dt
        # predict the next state using explicit Euler integration scheme
        y = y + dt * np.array(car_model(y, delta, a))

    return time, results

if __name__ == '__main__':
    # Define the system matrix A
    A = np.array([[1, 2],
                [-2, 1]])

    continuous_eigenvalues, discretized_eigenvalues, exp_A, x1_vals, x2_vals = linear_ode(A)

    print("Continuous-time eigenvalues:", continuous_eigenvalues)
    print("Discretized eigenvalues:", discretized_eigenvalues)

    # Plot the phase portrait
    plt.figure(figsize=(8, 6))
    plt.plot(x1_vals, x2_vals, label="Phase Portrait")
    plt.scatter(x1_vals[0], x2_vals[0], color='r', label="Initial State")
    plt.xlabel('$x_1$')
    plt.ylabel('$x_2$')
    plt.title('Phase Portrait of the System')
    plt.legend()
    plt.grid()
    plt.show()

    # Then Simulate the Time-varying Input
    time, results =  sim_car_euler()

    # Plotting
    plt.figure(figsize=(12, 8))

    plt.subplot(2, 2, 1)
    plt.plot(time, results[:, 0], label='x')
    plt.xlabel('Time')
    plt.ylabel('Position x')
    plt.legend()

    plt.subplot(2, 2, 2)
    plt.plot(time, results[:, 1], label='y', color='orange')
    plt.xlabel('Time')
    plt.ylabel('Position y')
    plt.legend()

    plt.subplot(2, 2, 3)
    plt.plot(time, results[:, 2], label='psi', color='green')
    plt.xlabel('Time')
    plt.ylabel('Heading psi')
    plt.legend()

    plt.subplot(2, 2, 4)
    plt.plot(time, results[:, 3], label='v', color='red')
    plt.xlabel('Time')
    plt.ylabel('Velocity v')
    plt.legend()

    plt.tight_layout()
    plt.show()
