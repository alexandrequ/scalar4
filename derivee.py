import numpy as np
import matplotlib.pyplot as plt
import random as rd

def f(x):
    return np.sin(1/x)

def derf(x):
    return -1/x**2 *np.cos(1/x)

def finitediff1(y):
    n=len(y)
    h=1/N
    result=[(y[1]-y[0])/h]
    for i in range(2,n):
        result.append((0.5*y[i-2]-2*y[i-1]+3/2*y[i])/h)
    return result

def finitediff2(y):
    n=len(y)
    h=1/N
    result=[]
    for i in range(1,n):
        result.append((y[i]-y[i-1])/h)
    return result

N=100
x=np.linspace(0.01,1,N)
y=f(x)

y1=finitediff1(y)
y2=finitediff2(y)
epsilon1=np.array(y1)-derf(x)[1:]
epsilon2=np.array(y2)-derf(x)[1:]

plt.plot(x[1:N],y1,label="difference finie ordre 2")
plt.plot(x[1:N],y2,label="difference finie ordre 1")
plt.plot(x[1:],epsilon1,label="erreur ordre 2")
plt.plot(x[1:],epsilon2,label="erreur ordre 1")
plt.legend()
plt.show()