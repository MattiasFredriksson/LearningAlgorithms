import numpy as np
import sys
import skimage.color
import matplotlib.pyplot as plt
from matplotlib import colors as mc
from matplotlib.ticker import NullFormatter
from mpl_toolkits.mplot3d import Axes3D
from time import time

direc = ""
col = mc.CSS4_COLORS
col = [col['blue'], col['orange'], col['green'], col['red'], col['purple'], col['brown'], col['pink'], col['gray'], col['olive'], col['cyan']]
for i in range(len(col)):
    col[i] = mc.to_rgb(col[i])
labels = ('0','1','2','3','4','5','6','7','8','9');

l = np.load(direc + "labels.npy")
lc = skimage.color.label2rgb(l, colors=col)

Y = np.load(direc + "tsne.npy")

print("Data dim.: ", Y.shape[0])
print("Features: ", Y.shape[1])

fig = plt.figure(figsize=(12, 12))
ax = fig.add_subplot(1, 1, 1)
plt.scatter(Y[:, 0], Y[:, 1], c=lc, marker='.', cmap=plt.cm.Spectral)
#plt.title("t-SNE")
ax.xaxis.set_major_formatter(NullFormatter())
ax.yaxis.set_major_formatter(NullFormatter())

handlelist = [plt.plot([], marker=".", ls="", ms=14, color=color)[0] for color in col]
plt.legend(handlelist,labels,loc='upper right',bbox_to_anchor=(1.105, 1), prop={'size': 14})

fig.savefig('TSNE.png')
#plt.show()
