import numpy as np
import skimage, os
from skimage import io, color, transform


def imshow(img):
    io.imshow(img)
    io.show()
    print("Image closed.")
    return;

#Single image ex:
RGB = io.imread("testing/image_0001.jpg")
RGB = skimage.util.img_as_float(RGB)
GRAY = color.rgb2gray(RGB)
TRANSFORMED = transform.resize(GRAY, [200,200], mode='constant')
imshow(TRANSFORMED)
