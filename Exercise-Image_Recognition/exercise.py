import numpy as np
import skimage, os
from skimage import io, color, transform
import sklearn
from sknn.mlp import Classifier, Layer, Convolution

def imshow(img):
    io.imshow(img)
    io.show()
    print("Image closed.")
    return;

def loadFile(path, file_ext = ".jpg") :
    files = []
    #Find and load files
    for file in os.listdir(path):
        if file.endswith(".jpg"):
            files.append(os.path.join(path, file))
    IMG_COL = io.ImageCollection(files, True)
    return IMG_COL;
#end

#Process
def process_gray(img_collection, size) :
    IMG_SIZE = 100
    IMGS = []
    for img in img_collection :
        img = color.rgb2gray(img)
        img = transform.resize(img, [size,size], mode='constant')
        IMGS.append(img)
    #end
    return IMGS
#end

# Convert array of images to a single numpy data array
def img2numpy(img_arr, width=-1, height=-1, size=-1):
    #Determine image size params
    if size != -1 : width = size; height = size
    if width == -1 : width = img_arr[0].shape[0]
    if height == -1 : height = img_arr[0].shape[1]

    #Process
    arr = np.empty(shape=[len(img_arr), height, width])
    for i in range(len(img_arr)) :
        img = img_arr[i].data
        for row in range(width):
            for col in range(height):
                arr[i, row, col] = img[row, col]
    return arr
#end

#Classification part:
from sklearn.neural_network import MLPClassifier

X = img2numpy(process_gray(loadFile("training"), 100))
X_test = img2numpy(process_gray(loadFile("testing"), 100))
y = np.repeat([0,1,2], [15,15,20])
y_test = np.repeat([1,0,2], [15,15,15])

#data = bunch(X, y)
#imshow(X[0])


nn = Classifier(
    layers=[
        Convolution('Rectifier', channels=12, kernel_shape=(3, 3), border_mode='full'),
        Convolution('Rectifier', channels=8, kernel_shape=(3, 3), border_mode='valid'),
        Layer('Rectifier', units=64),
        Layer('Softmax')],
    learning_rate=0.002,
    valid_size=0.2,
    n_stable=10,
    verbose=True)

nn.fit(X, y)

y_pred = nn.predict(X_test)
print(y_pred)

# Some result:
success = 0; fail = 0
for i in range(len(y_pred)) :
    if y_pred[i] == y_test[i] : success += 1
    else: fail += 1
#end
print("Success: ", success, "\n Fail: ", fail)
