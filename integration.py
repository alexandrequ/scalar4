import numpy as np
import matplotlib.pyplot as plt

def f(x):
    return np.sqrt(x)

def intf(a,x):
    return 2/3*(x**(3/2)-a**(3/2))

def simpson(y,a,b,N):
    sum=0
    pas=(b-a)/(N-1)*2
    i=0
    result=[0]
    while i+2<N:
        sum+=pas/6*(y[i]+4*y[i+1]+y[i+2])
        result.append(sum)
        i+=2
    if(N%2==0):
        sum+=(-1/24*y[-3]+1/3*y[-2]+5/24*y[-1])*pas
        result.append(sum)
    return result

def rectangle(y,a,b,N):
    sum=0
    result=[0]
    pas=(b-a)/(N-1)
    for i in range(1,N):
        sum+=y[i]*pas
        result.append(sum)
    return result

a=0
b=100
N=200
x=np.linspace(a,b,N)
if(N%2==0):
    x1=np.linspace(a,b-(b-a)/N,int(N/2))
    x1=np.concatenate([x1,[b]])
else:
    x1=np.linspace(a,b,int(N/2)+1)
y=f(x)
s1=simpson(y,a,b,N)
s2=rectangle(y,a,b,N)
s3=intf(a,x)

plt.plot(x1,s1,label="simpson")
plt.plot(x,s2,label="rectangle")
plt.plot(x,s3,label="solution exacte")
plt.legend()
plt.show()