import numpy as np
import sys
import skimage.color
import matplotlib.pyplot as plt
from matplotlib import colors as mc
from matplotlib.ticker import NullFormatter
from mpl_toolkits.mplot3d import Axes3D
from time import time

direc = ""
filen = "data5"

filter_with = 0
rec = -1
try:
   filen = sys.argv[1]
except:
    print('param: filen= ' + filen)
try:
   rec = int(sys.argv[2])
except:
    print('param: rec= ' + str(rec))
try:
   filter_with = int(sys.argv[3])
except:
    print('param: filter_with= ' + str(filter_with))

#Colors
col = mc.CSS4_COLORS
col = [col['blue'], col['orange'], col['green'], col['red'], col['purple'], col['brown'], col['pink'], col['gray'], col['olive'], col['cyan']]
for i in range(len(col)):
    col[i] = mc.to_rgb(col[i])

labels = ('0','1','2','3','4','5','6','7','8','9');

l = np.load(direc + "labels.npy")
lc = skimage.color.label2rgb(l, colors=col)
Y = np.load(direc + filen + ".npy")

def plot(Y, l, lc, col, filter_A=0, filter_B=1, rec=-1):

    #Filter
    F = []
    for i in range(len(l)):
        if l[i] == filter_A or l[i] == filter_B:
            F.append(i)
    #end
    Y = Y[F]
    lc = lc[F]
    dim = Y.shape[1]

    #Filter max records
    if rec > 0 and rec < Y.shape[0]:
        Y = Y[:rec,:]
        lc = lc[:rec]
    #Color filter
    col = [col[filter_A], col[filter_B]]

    print("Data records: ", Y.shape[0])
    print("Features: ", Y.shape[1])

    fig, axes = plt.subplots(nrows=dim, ncols=dim, figsize=(30, 30))

    for i in range(dim):
        for j in range(dim):
            ax = axes[i, j]
            if i == j:
                ax.hist(Y[:,i], bins='auto', density='True')
                ax.set_ybound(lower=0, upper=1)
            else: #elif j < i:
                ax.scatter(Y[:, j], Y[:, i], c=lc, marker='.', cmap=plt.cm.Spectral)

            #Name axis
            if ax.is_first_col():
                ax.yaxis.set_label_position('left')
                ax.set_ylabel('Dim ' + str(i+1), fontsize=16, rotation='vertical')

            if ax.is_first_row():
                ax.xaxis.set_label_position('top')
                ax.set_xlabel('Dim ' + str(j+1), fontsize=16)
            #else: #Do nothing
            #plt.title("t-SNE")
            #ax.xaxis.set_major_formatter(NullFormatter())
            #ax.yaxis.set_major_formatter(NullFormatter())
    #end for

    ax = axes[0, dim-1]
    handlelist = [ax.plot([], marker="o", ms=18, ls="", color=color)[0] for color in col]
    ax.legend(handlelist,labels,loc='upper right',bbox_to_anchor=(1.75, 1), prop={'size': 36})
    fig.savefig('img/scatterMF_'+  filen + '_' + str(filter_A) + str(filter_B) +'.png')
    #plt.show()
#endf

for i in range(10):
    if filter_with != i:
        plot(Y, l, lc, col, filter_A=filter_with, filter_B=i, rec=rec)
#end
