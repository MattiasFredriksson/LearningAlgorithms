import numpy
from sklearn.utils import shuffle

arr = numpy.loadtxt("spambase.data", delimiter=",")

target = arr[:,57]
data = numpy.delete(arr, 57, axis=1)

#Shuffle
target, data = shuffle(target, data, random_state=712351923)

#Convert to boolean
tmp = []
for x in target:
    tmp.append(bool(x))
target = tmp

numpy.save("spambase_data.npy", data)
numpy.save("spambase_target.npy", target)
