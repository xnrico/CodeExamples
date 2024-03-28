import numpy as np
import matplotlib.pyplot as plt

import cvxpy as cp
import numpy as np
from scipy import integrate

def CarModel(t, x, Student_Controller, param):
    
    if t <= param["switch_time"]:
        param["v0"] = param["v01"]
    if t > param["switch_time"]:
        param["v0"] = param["v02"]
    
    A, b, P, q = Student_Controller(t, x, param)
    
    var = cp.Variable(2)
    prob = cp.Problem(cp.Minimize((1/2)*cp.quad_form(var, P)+ q.T @ var),
                     [A @ var <= b])
    prob.solve()
    
    u = var.value[0]        
    u = np.clip(u, -param["Cdg"] * param["m"], param["Cag"] * param["m"])
    
    dx = np.array([param["v0"] - x[1], 
                   u / param["m"]])
    return dx

def sim_vehicle(Student_Controller, param, y0):
    t0, t1 = 0, param["terminal_time"]                # start and end
    t = np.linspace(t0, t1, 200)  # the points of evaluation of solution
    # y0 = [250, 10]                   # initial value
    y = np.zeros((len(t), len(y0)))   # array for solution
    y[0, :] = y0
    r = integrate.ode( lambda t, x:CarModel(t, x, Student_Controller, param) ).set_integrator("dopri5")  # choice of method
    r.set_initial_value(y0, t0)   # initial values
    for i in range(1, t.size):
       y[i, :] = r.integrate(t[i]) # get one more value, add it to the array
       if not r.successful():
           raise RuntimeError("Could not integrate")
    
    ### recover control input ###
    u = np.zeros((200, 1))
    for k in range(200):
        if t[k] <= param["switch_time"]:
            param["v0"] = param["v01"]
        if t[k] > param["switch_time"]:
            param["v0"] = param["v02"]
            
        A, b, P, q = Student_Controller(t[k], y[k, :], param)
        var = cp.Variable(2)
        prob = cp.Problem(cp.Minimize((1/2)*cp.quad_form(var, P)+ q.T @ var),
                         [A @ var <= b])
        prob.solve()

        u[k] = var.value[0]

    v0 = t * 0
    v0[t <  param["switch_time"]] = param["v01"]
    v0[t >= param["switch_time"]] = param["v02"]
    Cdg = param["Cdg"]
    B   = y[:, 0] - 1.8 * y[:, 1] - 0.5 * (np.clip(y[:, 1] - v0, 0, np.inf))**2 / Cdg

    return t, B, y, u

def Student_Controller(t, x, param):

    vd = param["vd"]
    v0 = param["v0"]

    m = param["m"]
    Cag = param["Cag"]
    Cdg = param["Cdg"]
    
    D = x[0]
    v = x[1]

    lam = 300
    alpha = 0.28
    w = 5e6
    h = 1/2 * ((v - vd)**2)
    B = D - 1/2 * ((v0 - v)**2) / Cdg - 1.8*v

    P = np.array([[2, 0], [0, 2*w]])
    A = np.array([[(v - vd)/m, -1],
                  [(1.8 + (v-v0)/Cdg)/m, 0],
                  [1 , 0],
                  [-1 , 0],
                  [0, -1]
                  ])

    b = np.array([-lam*h, alpha*B + (v0-v), (Cag-1e-5)*m, Cdg*m, 0])
    q = np.zeros([2, 1])
    
    return A, b, P, q

if __name__ == '__main__':
    param = {"vd": 20.0, "v0": 10.0, "m": 2000, "Cag": 0.3 * 9.81, "Cdg": 0.8 * 9.81, "v01": 30, "v02": 10, "switch_time": 15, "terminal_time": 50}
    y0 = np.array([30, 25]) # [D(0), v(0)]
    t, B, y, u = sim_vehicle(Student_Controller, param, y0)

    plt.figure(figsize=(20, 3))
    plt.plot(t, B, label="Value of Barrier Function")
    plt.xlabel('$Time (s)$')
    plt.ylabel('$B$')
    plt.title('Safety Constraints')
    plt.legend()
    plt.grid()
    plt.show()

    plt.figure(figsize=(20, 3))
    plt.plot(t, y[:, 0], label="Distance to leading vehicle")
    plt.xlabel('$Time (s)$')
    plt.ylabel('$m$')
    plt.title('vehicle Distance')
    plt.legend()
    plt.grid()
    plt.show()

    plt.figure(figsize=(20, 3))
    plt.plot(t, y[:, 1], label="Ego vehicle Velocity")
    v_lead = t * 0
    v_lead[t <= param["switch_time"]] = param["v01"]
    v_lead[t >= param["switch_time"]] = param["v02"]
    plt.plot(t, v_lead, label="Leading vehicle Velocity")
    plt.plot(t, t * 0 + param["vd"], "--", label="Desired Velocity")

    plt.xlabel('$Time (s)$')
    plt.ylabel('$m.s^{-1}$')
    plt.title('vehicleicle velocity')
    plt.legend()
    plt.grid()
    plt.show()

    plt.figure(figsize=(20, 3))
    plt.plot(t, 0 * t - param["Cdg"], "k--", label="Cag")
    plt.plot(t, 0 * t + param["Cag"], "k--", label="Cdg")
    plt.plot(t, u / param["m"], label="Normalized Control Input")
    plt.xlabel('$Time (s)$')
    plt.ylabel('$m.s^{-2}$')
    plt.title('Normalized Control Inputs')
    plt.legend()
    plt.grid()
    plt.show()