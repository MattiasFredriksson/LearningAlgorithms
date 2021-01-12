import numpy as np
import functools
import sys
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from time import time

direc = ""
X = np.load(direc + "features.npy")

dim = 10
scale = False
try:
   dim = int(sys.argv[1])
except:
    print('param: dim = ' + str(dim))
try:
   dim = sys.argv[2] == 'True'
except:
    print('param: scale = ' + str(scale))

#Cull some data for code tests
#X = X[:1000,:]

print("Data dim.: ", X.shape[0])
print("Features: ", X.shape[1])


#Normalization scaler
if scale:
    scaler = StandardScaler()
    scaler.fit(X)
    X = scaler.transform(X)
    print('Applied: StandardScaler')

t0 = time()
pca = PCA(n_components=dim, svd_solver='full', whiten='false')
Y = pca.fit_transform(X)

print("Compute t-SNE: %.2g sec" % (time() - t0))
print('Dims: ', pca.n_components)
print('Singular values: ', pca.singular_values_)
#print('Mean: ', pca.mean_)
print('Explained variance: ', pca.explained_variance_)

np.save("pca"+str(dim)+".npy", Y)
