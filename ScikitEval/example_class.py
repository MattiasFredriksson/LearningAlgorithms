

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
from sklearn import neighbors, datasets, metrics

#Load
data = np.load("spambase_data.npy")
target = np.load("spambase_target.npy")
#Partition
split_row = [int(.9*len(target))]
X, validate_X = np.vsplit(data, split_row)
y, validate_y = np.split(target, split_row)
#train, validate, test = np.vsplit(data, [int(.6*len(df)), int(.8*len(df))])


h = .02  # step size in the mesh

# Create color maps
cmap_light = ListedColormap(['#FFAAAA', '#AAFFAA', '#AAAAFF'])
cmap_bold = ListedColormap(['#FF0000', '#00FF00', '#0000FF'])

for weights in ['uniform', 'distance']:
    print("Test: " + weights)
    # we create an instance of Neighbours Classifier and fit the data.
    clf = neighbors.KNeighborsClassifier(n_neighbors=5, weights=weights)
    clf.fit(X, y)

    y_res = clf.predict(validate_X)
    fmeasure = metrics.f1_score(validate_y, y_res, average='binary', pos_label=1)
    accuracy = metrics.accuracy_score(validate_y, y_res)
    print("f1: ", fmeasure)
    print("acc: ", accuracy)
