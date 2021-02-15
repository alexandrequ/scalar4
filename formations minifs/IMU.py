import numpy as np
from math import*
import matplotlib.pyplot as plt
import random as rd

def LowPass(data):
    fft=np.fft.fftshift(np.fft.fft(data))
    N=len(fft)
    for i in range(N):
        
        fft[i]*=1/(1+(i-N/2)/N/(1/10)*1j)
    return np.fft.ifft(np.fft.ifftshift(fft))


def integrer(list,pas):
    sum=[0]
    n=len(list)
    for i in range(n-1):
        sum.append((list[i+1]+list[i])*pas/2+sum[i-1])
    return sum


def getData():
    file=open("dataIMU.txt","r")
    
    string=file.read()
    
    data=[]
    i=0
    n=len(string)
    while(i<n):
        cur_data=[]
        j=0
        while(j!=3):
            current=""
            
            while(i<n and string[i]!='\n'):
                current+=string[i]
                i+=1
            cur_data.append(float(current))
            i+=1
            j+=1
        data.append(cur_data)
    
    n=len(data)
    datax=np.zeros((n))
    datay=np.zeros((n))
    dataz=np.zeros((n))
    for i in range(n):
        datax[i]=data[i][0]
        datay[i]=data[i][1]
        dataz[i]=data[i][2]
    return datax, datay, dataz
        
## main program
accx,accy,accz=getData()
N=len(accx) #nombre de mesure
pas=0.1 #duree entre deux mesure
t=np.linspace(0,pas*(N-1),N)


x=integrer(integrer(LowPass(accx),0.1),0.1)
y=integrer(integrer(LowPass(accy),0.1),0.1)
x1=integrer(integrer(accx,0.1),0.1)
y1=integrer(integrer(accy,0.1),0.1)
plt.plot(x,y)
plt.plot(x1,y1,'.')

plt.show()