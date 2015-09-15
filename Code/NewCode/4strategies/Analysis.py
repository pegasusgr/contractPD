# Here I perform several analysis on the Inequality_efficieny results

import numpy
import matplotlib
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import matplotlib.colors as mc
import matplotlib.mlab as mlab
from matplotlib.ticker import NullFormatter
import matplotlib.patches as mpatches
from matplotlib.collections import PatchCollection
import matplotlib.path as mpath
import matplotlib.lines as mlines
from pylab import *
from numpy import array
from configobj import ConfigObj
from subprocess import call
import os

#****************************************Let's first of all get all the parameters:     **********************
config = ConfigObj("./parameters.txt")

dummy = config["N"]
N=int(dummy)
dummy = config["T"]
T=int(dummy)
dummy = config["L"]
L=int(dummy)
dummy = config["delta"]
delta=float(dummy)
dummy = config["beta"]
beta=float(dummy)
dummy = config["r"]
r=float(dummy)
dummy = config["a"]
a=float(dummy)

#***************************************************************************************

#**************************** Now I load everything: *****************

fuffo=numpy.loadtxt("./time.txt")
data=fuffo.transpose() # So now basically data[0] is time, data[1] is welfare and data[2-3-4] are the percentages


#******************************************************

stringa = "N= "+str(N)+"\nL= "+str(L)+"\ndelta= "+str(delta)+"\nbeta= "+str(beta)

#*************** Now let's make the plot for the welfare coefficient ************************
plt.plot(data[0], data[1])
plt.title("Welfare as a function of time")
plt.xlabel('time')
plt.ylabel('Welfare')
text(T-T/5, 20, stringa, bbox=dict(facecolor='orange', alpha=0.8))
plt.tight_layout()
plt.savefig('welfare.pdf',dpi=100)
close()
#******************************************************************************

#********** Now let's make the plot for the percentages, 

#*********** Now let's plot the average total wealth (the score) for each time step.
plt.plot(data[0], data[2])
plt.plot(data[0], data[3])
plt.plot(data[0], data[4])
plt.title("Percentages of agents playing C, D and Delta as a function of time")
legend1=r'$C$'
legend2=r'$D$'
legend3=r'$\Delta$'
plt.legend((legend1,legend2,legend3),loc=2)
plt.xlabel('time')
plt.ylabel('percentages')
text(T-T/5, 0.8, stringa, bbox=dict(facecolor='orange', alpha=0.8))
plt.tight_layout()
plt.savefig('percentages.pdf',dpi=100)
close()



