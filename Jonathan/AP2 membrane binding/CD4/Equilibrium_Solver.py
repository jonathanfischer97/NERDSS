from sympy.solvers import solve
from sympy import symbols
import numpy as np
from sympy import nsolve

#Define variables
a,p,c,t,ap,apt,apc,aptc = symbols("a,p,c,t,ap,apt,apc,aptc")

#Define rates
k_on = np.array([0.1, 0.1362/2, 0.01362/2, 0.0136/2, 0.1362/2])
k_off = np.array([33, 48.17, 48.17, 48.17, 48.17])

#Define initial solution to start with
init_conc = [500,1000,100,100,0,0,0,0]
init_sol = np.random.rand(len(init_conc))*max(init_conc)

#Rate Equations
eq1 = -k_on[0]*a*p + k_off[0]*ap
eq2 = - k_on[1]*ap*t + k_off[1]*apt
#eq3 = -k_on[2]*ap*c + k_off[2]*apc 
eq4 = -k_on[3]*apt*c + k_off[3]*aptc
eq5 = - k_on[4]*apc*t + k_off[4]*aptc

c1 = a+ap+apc+apt+aptc-500.0   #A
c2 = p+ap+apc+apt+aptc-1000.0    #P
c3 = c+apc+aptc-100.0           #T
c4 = t+apt+aptc-100.0           #C

system = [eq1,eq2,eq4,eq5,c1,c2,c3,c4]

print(init_sol)
print(system)

solution = nsolve(system,[a,p,c,t,ap,apt,apc,aptc],init_sol,verify=True)

print("The equilibrium solution is: ")
print(solution)
