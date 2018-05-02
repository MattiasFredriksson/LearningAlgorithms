import numpy
from sklearn.utils import shuffle

#Files
feature_file = "features_train"
label_file = "label_train"
feature_test_file = "features_test"
label_test_file = "label_test"

#Parse training data

data = numpy.loadtxt(feature_file+".txt", delimiter=",")
target = numpy.loadtxt(label_file+".txt", delimiter=",")

#Shuffle
target, data = shuffle(target, data, random_state=712351923)

print("Dim feat: ", data.shape[0], ", " , data.shape[1])
print("Dim label: ", target.shape[0])
numpy.save(feature_file+".npy", data)
numpy.save(label_file+".npy", target)

#Repeat for test data

data = numpy.loadtxt(feature_test_file+".txt", delimiter=",")
target = numpy.loadtxt(label_test_file+".txt", delimiter=",")

#Shuffle
target, data = shuffle(target, data, random_state=712351923)

print("Dim feat: ", data.shape[0], ", " , data.shape[1])
print("Dim label: ", target.shape[0])
numpy.save(feature_test_file+".npy", data)
numpy.save(label_test_file+".npy", target)
