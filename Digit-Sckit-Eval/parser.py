import numpy
from sklearn.utils import shuffle

feature_file = "feature_few/features_test"
label_file = "feature_few/label_test"

data = numpy.loadtxt(feature_file+".txt", delimiter=",")
target = numpy.loadtxt(label_file+".txt", delimiter=",")

#Shuffle
target, data = shuffle(target, data, random_state=712351923)

print("Dim feat: ", data.shape[0], ", " , data.shape[1])
print("Dim label: ", target.shape[0])
numpy.save(feature_file+".npy", data)
numpy.save(label_file+".npy", target)
