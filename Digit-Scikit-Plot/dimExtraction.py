import numpy as np
import functools
import sys
from sklearn.preprocessing import StandardScaler
from sklearn.decomposition import PCA
from time import time

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

direc = ""
X = np.load(direc + "features.npy")

dim = 10
scale = False
try:
   dim = int(sys.argv[1])
except:
    print('param: dim=10')
try:
   scale = str2bool(sys.argv[2])
except:
    print('param: scale=False')

print("Data dim.: ", X.shape[0])
print("Features: ", X.shape[1])

#Cull columns
X = X[:,:dim]


print("\nOut dim.: ", X.shape[1])

#Normalization scaler
if scale:
    scaler = StandardScaler()
    scaler.fit(X)
    X = scaler.transform(X)
    print("StandardScaler applied")

np.save("data"+str(dim)+".npy", X)
