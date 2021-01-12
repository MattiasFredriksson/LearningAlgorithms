import numpy as np
import functools
import sys
from sklearn.preprocessing import StandardScaler
from sklearn import manifold
from time import time

direc = ""
X = np.load(direc + "features.npy")

nNeighbor = 20
dim = 2
try:
   nNeighbor = int(sys.argv[1])
except:
    print('param: nNeighbor= ' + str(nNeighbor))

#Cull some data for code tests
#X = X[:1000,:]

print("Data dim.: ", X.shape[0])
print("Features: ", X.shape[1])


#Normalization scaler
scaler = StandardScaler()
scaler.fit(X)
X = scaler.transform(X)

t0 = time()
mfold = manifold.TSNE(n_components=dim, perplexity=nNeighbor, n_iter=1000, init='pca', method='barnes_hut', random_state=0)
#mfold = manifold.LocallyLinearEmbedding(nNeighbor, dim, eigen_solver='auto', method='modified')
#mfold = manifold.SpectralEmbedding(n_components=2, n_neighbors=nNeighbor)
t1 = time()
Y = mfold.fit_transform(X)

print("Compute t-SNE: %.2g sec" % (t1 - t0))
np.save("tsne" + str(nNeighbor) + ".npy", Y)
