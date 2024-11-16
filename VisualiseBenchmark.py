import numpy as np
import matplotlib.pyplot as plt
from numpy.core.fromnumeric import mean

data = np.loadtxt("benchmark.txt")

#skip first frame
it = data[2:-1,0]
time = data[2:-1,1]
cycles = data[2:-1,2]

fig = plt.figure()
ax1 = fig.add_subplot(1, 2, 1)
fig.subplots_adjust(bottom=0.3)
ax1.plot(it, time)
plt.xlabel("Iterations")
plt.ylabel("Microseconds")

ax2 = fig.add_subplot(1,2,2)
ax2.plot(it, cycles)
plt.xlabel("Iterations")
plt.ylabel("Mega Cycles")

textstr = '\n'.join("Jcdbcbsdiuc")
fig.text(0.10, 0.02, 
         f"""Mean MegaCycles per frame = {cycles.mean():.3f}\n
Mean FPS = {it[-1]/(time.sum() * 1e-3):.3f} \n 
Mean Miliseconds Per Frame {time.mean():.3f}""", wrap=True ) 

plt.show()
