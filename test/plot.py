from __future__ import unicode_literals
import matplotlib.pyplot as plt
import numpy as np
import os
import matplotlib as mpl
mpl.use("pgf")


f = open("test/time.dat", "w")

print("Calculating polynomials of degree 8, 16, 32, 64, 128 and 256...")
f.write("#number of polynomials    time parallel 8    time parallel 16    time parallel 32    time parallel 64    time parallel 128    time parallel 256    time serial 32\n")

for i in [8960,17920,35840,71680,143360]:
    print("Calculating " + str(i) + " polynomials...")

    command = str(i)+" "
    
    stream = os.popen(" bin/ws_ocl "+str(i)+" 8 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws_ocl "+str(i)+" 16 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws_ocl "+str(i)+" 32 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws_ocl "+str(i)+" 64 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws_ocl "+str(i)+" 128 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws_ocl "+str(i)+" 256 -1 1 0.0001")
    command += stream.read()
    stream = os.popen(" bin/ws "+str(i)+" 32 -1 1 0.0001")
    command += stream.read()
    command = command.replace("\n"," ")
    
    f.write(command+"\n")

f.close()


data = np.loadtxt('test/time.dat')
y8   = data[:,1]
y16  = data[:,2]
y32  = data[:,3]
y64  = data[:,4]
y128 = data[:,5]
y256 = data[:,6]
s    = data[:,7]
x    = data[:,0]

#plt.axis([8000*32, 57345000, 1, 220])

plt.plot( 8*x, y8, 'o', markerfacecolor='white',markeredgecolor='red', label='degree 8')
plt.plot( 8*x, y8, 'o', markerfacecolor='red',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(8*x, y8, 1)
print(fit)

plt.plot(16*x, y16, 'o', markerfacecolor='white',markeredgecolor='blue', label='degree 16')
plt.plot(16*x, y16, 'o', markerfacecolor='blue',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(16*x, y16, 1)
print(fit)

plt.plot(32*x, y32, 'o', markerfacecolor='white',markeredgecolor='green', label='degree 32')
plt.plot(32*x, y32, 'o', markerfacecolor='green',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(32*x, y32, 1)
print(fit)

plt.plot(64*x, y64, 'o', markerfacecolor='white',markeredgecolor='orange', label='degree 64')
plt.plot(64*x, y64, 'o', markerfacecolor='orange',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(64*x, y64, 1)
print(fit)

plt.plot(128*x, y128, 'o', markerfacecolor='white',markeredgecolor='yellow', label='degree 128')
plt.plot(128*x, y128, 'o', markerfacecolor='yellow',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(128*x, y128, 1)
print(fit)

plt.plot(256*x, y256, 'o', markerfacecolor='white',markeredgecolor='gray', label='degree 256')
plt.plot(256*x, y256, 'o', markerfacecolor='gray',markeredgecolor='none',alpha=0.5)
fit = np.polyfit(256*x, y256, 1)
print(fit)

plt.xscale('log')
plt.yscale('log')
plt.ylabel('time / seconds')
plt.xlabel('number of zero points')
plt.legend(loc='upper left', numpoints=1)

plt.savefig('test/timedeg.pgf', dpi=None, facecolor='w', edgecolor='w', orientation='portrait', format='pgf', transparent=True, bbox_inches=None, pad_inches=0.1)

plt.close()

###################################################

plt.plot(x, y32, 'o', markerfacecolor='white',markeredgecolor='red', label='GPU')
plt.plot(x, y32, 'o', markerfacecolor='red',markeredgecolor='none',alpha=0.5)

plt.plot(x, s,   'o', markerfacecolor='white',markeredgecolor='green', label='CPU')
plt.plot(x, s,   'o', markerfacecolor='green',markeredgecolor='none',alpha=0.5)

fit1 = np.polyfit(x, s, 1)
print(fit1)
plt.plot(x, fit1[0]*x+fit1[1], color='green')

fit2 = np.polyfit(x, y32, 1)
print(fit2)
plt.plot(x, fit2[0]*x+fit2[1], color='red')

plt.xscale('log')
plt.yscale('log')
plt.ylabel('time / seconds')
plt.xlabel('number of polynomials')
plt.legend(loc='upper left', numpoints=1)

plt.savefig('test/timeproc.pgf', dpi=None, facecolor='w', edgecolor='w', orientation='portrait', format='pgf', transparent=True, bbox_inches=None, pad_inches=0.1)

print('Speedup: ')
print(fit1[0]/fit2[0])

plt.close()
