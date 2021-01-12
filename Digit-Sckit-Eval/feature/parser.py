import numpy
import sys
from sklearn.utils import shuffle

def str2bool(v):
  return v.lower() in ("yes", "true", "t", "1")

#Files
feature_file = "features_train"
label_file = "label_train"
feature_test_file = "features_test"
label_test_file = "label_test"

rec=-1
parseTest = False;
try:
   rec = int(sys.argv[1])
except:
    print('param: rec='+str(rec))
try:
   parseTest = str2bool(sys.argv[2])
except:
    print('param: parseTest='+str(parseTest))

#Parse training data

data = numpy.loadtxt(feature_file+".txt", delimiter=",")
target = numpy.loadtxt(label_file+".txt", delimiter=",")

#Shuffle
target, data = shuffle(target, data, random_state=712351923)

if rec < 0 or rec > target.shape[0]:
    rec = target.shape[0]

target = target[0:rec]
data = data[0:rec]

print("Dim feat: ", data.shape[0], ", " , data.shape[1])
print("Dim label: ", target.shape[0])
numpy.save(feature_file+".npy", data)
numpy.save(label_file+".npy", target)

#Repeat for test data
if parseTest:
    data = numpy.loadtxt(feature_test_file+".txt", delimiter=",")
    target = numpy.loadtxt(label_test_file+".txt", delimiter=",")

    #Shuffle
    target, data = shuffle(target, data, random_state=712351923)

    print("Dim feat: ", data.shape[0], ", " , data.shape[1])
    print("Dim label: ", target.shape[0])
    numpy.save(feature_test_file+".npy", data)
    numpy.save(label_test_file+".npy", target)
