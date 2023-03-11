import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
import pickle
from numpy import newaxis
import sklearn
import math
import cv2

def augment(img, horizontal_flip=True, vertical_flip = False, gaussian_blur=False, gaussian_noise=False, rotate_angle=0, perspective_transform_ratio = 1.0):
    rows, cols, _ = img.shape
    if horizontal_flip:
      img = cv2.flip(img,1)
    if vertical_flip:
      img = cv2.flip(img,0)
    if gaussian_blur:
      img = cv2.GaussianBlur(img,(5,5),0) # 5x5 kernel, sigmaX=0
    if gaussian_noise:
      sigma = 0.357 ** 0.5 # variance = 0.357
      gaussian = np.random.normal(0,sigma,(rows, cols)) # mean = 0
      img = np.zeros(img.shape, np.float32)
      img[:, :, 0] = img[:, :, 0] + gaussian
      img[:, :, 1] = img[:, :, 1] + gaussian
      img[:, :, 2] = img[:, :, 2] + gaussian
      cv2.normalize(img, img, 0, 255, cv2.NORM_MINMAX, dtype=-1)
      img = img.astype(np.uint8)
    #rotate
    M = cv2.getRotationMatrix2D((cols/2,rows/2), rotate_angle, 1)
    img = cv2.warpAffine(img, M, (cols,rows))
    #perspective transform
    if perspective_transform_ratio == 1.0:
      return img
    input_pts=np.float32([[0, 0], [img.shape[0], 0], [0, img.shape[1]], [img.shape[0], img.shape[1]]])
    width = int(round(img.shape[0]*perspective_transform_ratio))
    output_pts=np.float32([[0, 0], [width, 0], [0, img.shape[1]], [width, img.shape[1]]])
    M = cv2.getPerspectiveTransform(input_pts, output_pts)
    img = cv2.warpPerspective(img, M, (img.shape[0], img.shape[1]))
    img = img[:, 0:width]
    return img
  
def unpickle(file):
    import pickle
    with open(file, 'rb') as fo:
        dict = pickle.load(fo, encoding='bytes')
    return dict

data_batches = []
for i in range(1,6): # 6 data batches of 10k each. concat them together
  file = f'cifar/data_batch_{i}'
  data_batches.append(unpickle(file))
test_batch = unpickle('cifar/test_batch')
test_data1 = test_batch[b'data']
print(f"test data shape: {test_data1.shape}")
test_labels1 = test_batch[b'labels']
print(len(test_labels1))

train_data1 = np.concatenate((data_batches[0][b'data'], data_batches[1][b'data'], data_batches[2][b'data'], data_batches[3][b'data'], data_batches[4][b'data']), axis=0)
print(f"train data shape: {train_data1.shape}")
train_labels1 = np.concatenate((data_batches[0][b'labels'], data_batches[1][b'labels'], data_batches[2][b'labels'], data_batches[3][b'labels'], data_batches[4][b'labels']), axis=0)
print(train_labels1.shape)

# augment data
train_data_augmented = []
for i in range(train_data1.shape[0]):
  img = train_data1[i].reshape(3,32,32).transpose(1,2,0) # convert to 3x32x32 to 32x32x3 
  img_augmented = augment(img).transpose(2,0,1).reshape(3072) # augment then convert back to 1x3072
  train_data_augmented.append(img_augmented)
train_data_augmented = np.array(train_data_augmented)
#concatenate original data with augmented data to create a larger dataset!!! :D
train_data_augmented = np.concatenate((train_data1, train_data_augmented), axis=0)
train_labels_augmented = np.concatenate((train_labels1, train_labels1), axis=0)
print("augmented shape: ",train_data_augmented.shape)

print("row 0 to 5: original, horizontal flip, vertical flip, perspective transform 0.753, gaussian blur, rotate 45")
indices = np.random.randint(0, train_data_augmented.shape[0], 8)
#plot
fig, axes = plt.subplots(ncols=4, nrows=6, constrained_layout=True, figsize=(32, 32))

for x in range(0,4):
  img = train_data1[indices[x]].reshape(3,32,32).transpose(1,2,0)
  axes[0][x].imshow(img)
  hflipped = augment(img, horizontal_flip=True)
  axes[1][x].imshow(hflipped)
  vflipped = augment(img, horizontal_flip = False, vertical_flip=True)
  axes[2][x].imshow(vflipped)
  perspectiveTransform =  augment(img, horizontal_flip = False, vertical_flip=False, perspective_transform_ratio=0.753)
  axes[3][x].imshow(perspectiveTransform)
  blur = augment(img, horizontal_flip = False, vertical_flip=False, gaussian_blur=True)
  axes[4][x].imshow(blur)
  rotate = augment(img, horizontal_flip = False, vertical_flip=False, rotate_angle=45)
  axes[5][x].imshow(rotate)
plt.show()
