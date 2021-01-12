import numpy as np
import sys, math
import skimage.color
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.ticker import NullFormatter
from mpl_toolkits.mplot3d import Axes3D
from time import time
from sklearn.preprocessing import MinMaxScaler

direc = ""
labels = ('0','1','2','3','4','5','6','7','8','9');

dim = 256
l_heat = 6
use_alpha = False # Blending density
plt_cbar = False
try:
   plt_cbar = sys.argv[1] == 'True'
except:
    print('param: plt_cbar='+str(plt_cbar))


l = np.load(direc + "labels.npy")



Y = np.load(direc + "tsne.npy")
#Normalize data for mapping into the plot resolution
scaler = MinMaxScaler(copy=False, feature_range=(0, 1))
Y = scaler.fit_transform(Y)

print("Data dim.: ", Y.shape[0])
print("Features: ", Y.shape[1])

def plot( dim, digit, use_alpha=False, clear=True, plt_cbar=False, bg_col=1):

    # Count occurance of labels:
    count_h = np.zeros((dim, dim))
    count_c = np.zeros((dim, dim))
    count = np.zeros((dim, dim))
    max_count = 0
    max_h = 0
    dim_fit = dim - 1
    for i in range(Y.shape[0]):
        x = math.floor(Y[i, 0]*dim_fit)
        y = math.floor(Y[i, 1]*dim_fit)
        # Increment counters
        count[y, x] += 1;
        if l[i] == digit:
            count_h[y, x] += 1;
        else:
            count_c[y, x] += 1;
        # Find the maximum point allocations
        max_count = max(max_count, count[y, x])
        max_h = max(max_h, count_h[y, x])
    #end

    dens_mean = count.mean()
    std_dev = count.std()
    dens_div = 1
    if use_alpha:
        dens_div = dens_mean + std_dev * 2
    print("Max count: ", max_count)
    print("Max heat: ", max_h)
    print("Avg. Count: ", dens_mean)
    print("Stdev. Count: ", std_dev)

    density = np.zeros((dim, dim))
    heat = np.zeros((dim, dim))
    for i in range(dim):
        for j in range(dim):
            c = count_c[i, j] + count_h[i, j]
            heat[i,j] = count_h[i, j] / max(1,c) # Avoid div by 0
            density[i,j] = min(1, c/dens_div)
    #end

    cmap = plt.cm.get_cmap(name='jet')
    heatmap = cmap(heat.data)
    heatmap[..., -1] = density # Alpha

    bg = np.array([bg_col, bg_col, bg_col], dtype=np.float32)
    bg = np.tile(bg,(dim,1, 1))
    bg = np.tile(bg,(1,dim, 1))
    #bg = np.ones((dim, dim, 3))

    fig = plt.figure(figsize=(12, 12))
    ax = fig.add_subplot(1, 1, 1)
    im = ax.imshow(bg)
    im_fig = ax.imshow(heatmap, cmap=cmap, interpolation='gaussian', origin='lower')
    #plt.title("t-SNE Heatmap")
    # Color bar annotatio
    if plt_cbar:
        ax_cb = plt.colorbar(im_fig)
        ax_cb.ax.set_ylabel('Ratio of digit ' + str(digit), rotation=-90, va="bottom")

        ax1 = fig.add_axes([0.135, 0.06, 0.6, 0.03])

        if use_alpha:
            cmap = mpl.cm.gray
            norm = mpl.colors.Normalize(vmin=0, vmax=dens_div)
            cb = mpl.colorbar.ColorbarBase(ax1, cmap=cmap, norm=norm, orientation='horizontal')
            cb.set_label('Number of records')
            cb.ax.set_title('> ' + str(math.floor(dens_div)), loc='right')
        elif bg_col < 1:
            cmap = mpl.colors.ListedColormap([[bg_col, bg_col, bg_col], [bg_col, bg_col, bg_col], [1,1,1]])
            bound = [0,0.5,1]
            norm = mpl.colors.BoundaryNorm(bound, cmap.N)
            cb = mpl.colorbar.ColorbarBase(ax1, cmap=cmap, norm=norm,
                                        spacing='proportional',
                                        boundaries=bound, ticks=bound, orientation='horizontal')
            cb.set_label('Number of records')
            cb.ax.set_xticklabels(['0', '', '>= 1'])
    #endif

    # No axis annotation
    ax.xaxis.set_major_formatter(NullFormatter())
    ax.yaxis.set_major_formatter(NullFormatter())

    fig.savefig('Digit'+ str(digit) + '.png')
    #plt.show()

    if clear:
        fig.clf()
#endfunc

for i in range(10):
    plot(dim, i, use_alpha=False, plt_cbar=plt_cbar, bg_col=0)
