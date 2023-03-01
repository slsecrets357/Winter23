import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
import pickle
from numpy import newaxis
import sklearn
import math
import cv2

#function used to perform data augmentation. applies various augmentations to image depending on which input params are set to true
def augment(img, horizontal_flip=True, vertical_flip = False, gaussian_blur=False, gaussian_noise=False, rotate_angle=0, perspective_transform_ratio = 1.0):
    rows, cols, _ = img.shape
    if horizontal_flip:
      img = cv2.flip(img,1)
    if vertical_flip:
      img = cv2.flip(img,0)
    if gaussian_blur:
      img = cv2.GaussianBlur(img,(5,5),0) # 5x5 kernel, sigmaX=0
    if gaussian_noise: #dont use this xD
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

#show the effects of augmentation
#the images are so blurry lol
print("row 0 to 5: original, horizontal flip, vertical flip, perspective transform 0.753, gaussian blur, rotate 45")
indices = np.random.randint(0, train_data1.shape[0], 8) #generate random indices to show random images...
#plot
fig, axes = plt.subplots(ncols=4, nrows=6, constrained_layout=True, figsize=(20, 8))

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

#one hot encode
def one_hot(y, num_classes):
  return np.eye(num_classes)[y]
num_classes = 10
train_labels = one_hot(train_labels1, num_classes)
train_labels_augmented = one_hot(train_labels_augmented, num_classes)
#train_labels = np.array(train_labels1)
print(f"train labels shape: {train_labels.shape}")
print(f"augmented train labels shape: {train_labels_augmented.shape}")
test_labels = one_hot(test_labels1, num_classes)
#test_labels = np.array(test_labels1)
print(f"test labels shape: {test_labels.shape}")

#normalize :/
train_data = train_data1/255.
train_data_augmented = train_data_augmented/255.
test_data = test_data1/255.

#abstract class
class Layer:
  def __init__(self):
    self.input = None
    self.output = None
  def forward_prop(self, input):
    raise NotImplementedError
  def back_prop(self, output_error, alpha):
    raise NotImplementedError

#Fully Connected Layer
class FCL(Layer):
  def __init__(self, input_size, output_size, l1_lambda=0.0, l2_lambda=0.0):
    '''
        Initialize weights and bias
        params:
            input_size: number of input neurons
            output_size: number of output neurons
    '''
    self.weights = np.random.rand(input_size, output_size) - 0.5
    self.bias = np.random.rand(1, output_size) - 0.5
    self.l2_lambda = l2_lambda  # add regularization parameter
    self.l1_lambda = l1_lambda  # add regularization parameter
  def forward_prop(self, input_data):
    '''
        Perform forward propagation
        params:
            input_data: input data
        returns:
            output of the layer
    '''
    self.input = input_data
    self.output = np.dot(self.input, self.weights) + self.bias
    return self.output
  def back_prop(self, output_error, alpha):
    '''
        Perform backward propagation
        params:
            output_error: error of the output layer
            alpha: learning rate
            returns:
                input error of the layer
        output error is the derivative of the loss function wrt to the output of the layer
        input error is used to calculate the error of the previous layer
        weight gradient is the derivative of the loss function wrt to the weights,
        calculated using the chain rule: dE/dw = dE/dz * dz/dw. z is weight*input+bias
    '''
    input_error = np.dot(output_error, self.weights.T)
    weight_gradient = np.dot(self.input.T, output_error)
    bias_gradient = np.sum(output_error)
    
    # add L2 regularization term to weight gradient
    # weight penalty added to output error is: lambda/2*sum(weight^2)
    # derivative of L2 regularization term wrt to weights: lambda*weights
    weight_gradient += self.l2_lambda * self.weights # add L2 regularization term to weight gradient
    # weight penalty added to output error is: lambda*sum(abs(weight))
    # derivative of L1 regularization term wrt to weights: lambda*sign(weights)
    weight_gradient += self.l1_lambda * np.sign(self.weights) # add L1 regularization term to weight gradient
    
    #update parameters
    self.weights -= alpha*weight_gradient
    self.bias -= alpha*bias_gradient
    return input_error

#activation layer
class ActivationLayer(Layer):
  def __init__(self, activation, activation_prime): # apply non-linear activation to input
    self.activation = activation
    self.activation_prime = activation_prime
  def forward_prop(self, input_data):
    self.input = input_data
    self.output = self.activation(self.input)
    return self.output
  def back_prop(self, output_error, alpha): # Returns input error of activation function wrt to output error.
    '''
        params:
            output_error: error of the output layer
            alpha: learning rate (not used here)
        returns:
            input error of the layer
        output error is the derivative of the loss function wrt to the output of the layer
        input error is calculated using the chain rule: dE/dx = dE/dy * dy/dx
    '''
    return self.activation_prime(self.input) * output_error

# activation functions and their derivatives
def tanh(x):
    return np.tanh(x)
def tanh_prime(x):
    return 1-np.tanh(x)**2
def relu(x):
    '''
      Rectified Linear Unit activation function
      returns 0 if x<0, x if x>=0
    '''
    return np.maximum(x,0)
def relu_prime(x):
    '''
      Derivative of the Rectified Linear Unit activation function
      the derivative of relu is 1 if x>=0, 0 if x<0
    '''
    return x>0
def leaky_relu(x, alpha=0.01):
    '''
      Leaky Rectified Linear Unit activation function
      returns alpha*x if x<0, x if x>=0
    '''
    return np.maximum(x, alpha*x)
def leaky_relu_prime(x, alpha=0.01):
    '''
      Derivative of the Leaky Rectified Linear Unit activation function
      the derivative of leaky relu is 1 if x>=0, alpha if x<0
    '''
    return np.where(x > 0, 1, alpha)
def sigmoid(x):
    return 1 /(1 + np.exp(-x))
def sigmoid_prime(x):
    s = sigmoid(x)
    return s*(1 - s)
def softmax(z):
    '''
      Softmax activation function
      calculates the probability of each class
      expz is the exponential of the input
      y is the probability of each class
      divide numerator and denominator by max of z to avoid overflow: softmax = C*exp(z)/(C*sum(exp(z))), C = max(z)
      allclose checks if the sum of the probabilities is 1
    '''
    # expz = np.exp(z)
    expz = np.exp(z - np.max(z, axis=1, keepdims=True)) #seems more stable numerically .-.
    y = expz/np.sum(expz,axis=1,keepdims=True)
    if not np.allclose(np.sum(y, axis=1), 1):
        raise ValueError("Probabilities do not sum to 1 along class axis.")
    y = np.clip(y, 1e-12, 1 - 1e-12)
    return y
def softmax_prime(z):
    softm = softmax(z)
    y = softm*(1-softm)
    return y

#error functions
def mse(y_true, y_pred):
    return np.mean(np.power(y_true-y_pred, 2)) 
def mse_prime(y_true, y_pred):
    return 2*(y_pred-y_true)
def cross_entropy(y_true, y_pred, epsilon = 1e-12):  
    y_pred = np.clip(y_pred, epsilon, 1.0 - epsilon)
    return -np.mean(np.sum(y_true * np.log(y_pred), axis=1)) 
def cross_entropy_prime(y_true, y_pred, epsilon = 1e-12):
    y_pred = np.clip(y_pred, epsilon, 1.0 - epsilon) #add small value to avoid division by 0
    return -y_true/y_pred
def cross_entropy_with_sm(y_true,y_pred): #with softmax
    '''
    y_true is one-hot encoded vector
    calculates cross entropy loss by first applying softmax to y_pred
    then calculates cross entropy loss of y_true and y_pred
    '''
    
    p = softmax(y_pred)
    # print(y_true.shape, p.shape)
    log_likelihood = -np.log(np.sum(y_true*p, axis=1))
    loss = np.sum(log_likelihood) / y_true.shape[0]
    return loss
def cross_entropy_prime_with_sm(y_true,y_pred): 
    '''
    derivative of cross entropy loss with softmax is softmax(y_pred) - y_true
    gradient is calculated by dividing by number of samples
    '''
    grad = softmax(y_pred)
    grad -= y_true
    grad = grad/y_true.shape[0]
    return grad
#helper functions
#compares prediction to actual labels and computes accuracy
def accuracy(prediction, actual):
    if actual.ndim != 1:
      actual = np.argmax(actual, axis=1)
    print("calculating accuracy. prediction, actual: ", prediction.shape, actual.shape)
    size = prediction.shape[0]
    return np.sum(prediction == actual)/size

class Network:
    def __init__(self, epsilon=1e-4, loss = mse, loss_prime = mse_prime, l1_lambda = 0, l2_lambda = 0):
        self.layers = []
        self.loss = loss
        self.loss_prime = loss_prime
        self.epsilon = epsilon
        np.random.seed(0)
        self.l1_lambda = l1_lambda
        self.l2_lambda = l2_lambda

    # append layer
    def add(self, layer):
        self.layers.append(layer)

    # set error function
    def change_loss(self, loss, loss_prime):
        self.loss = loss
        self.loss_prime = loss_prime
    
    #takes an m-example input and returns m-result output
    def predict(self, input_data):
        output = input_data
        for layer in self.layers:
            output = layer.forward_prop(output)
        return np.argmax(output, axis=1)

    def fit(self, x, y, num_epochs, alpha, optimizer=None, batch_size = 32, val_data = None, val_labels=None):
      '''
        optimize the network using minibatch stochastic gradient descent
        params:
            x: input data
            y: labels
            num_epochs: number of epochs
            alpha: learning rate
            optimizer: optimizer to use (not implemented yet)
            batch_size: batch size
            val_data: validation data
            val_labels: validation labels
      '''
      num_it = math.ceil(x.shape[0]/ batch_size) #number of iterations per epoch
      print(f"optimizing with gradient descent, batch size = {batch_size}, num_it={num_it}")
      indices = np.random.permutation(x.shape[0]) #shuffle data
      x1, y1 = x[indices], y[indices] #shuffled data
      if val_data is None or val_labels is None: #if no validation data is given, use 10% of train data as validation data
        print("validation data not given. using 10% of train data as val")
        val_data, val_labels = x1[0:int(x1.shape[0]/10)], y1[0:int(y1.shape[0]/10)]
      for t in range(num_epochs):
        error = 0
        indices = np.random.permutation(x.shape[0]) #shuffle data
        x_shuffle, y_shuffle = x1[indices], y1[indices] #shuffled data
        for i in range(0, x.shape[0], batch_size): #iterate over batches
          x_batch = x_shuffle[i:i+batch_size] #get data batch
          y_batch = y_shuffle[i:i+batch_size] #get label batch
          output = x_batch 
          N,D = x_batch.shape
          # forward propagation
          # l2_reg = 0
          # l1_reg = 0
          error_prime = 0
          for layer in self.layers: #iterate over layers
            output = layer.forward_prop(output)
            # if isinstance(layer, FCL):
            #   l2_reg += np.sum(layer.weights**2) 
            #   l1_reg += np.sum(np.abs(layer.weights))
            #   error_prime += self.l2_lambda * np.sum(layer.weights) / N #add l2 regularization term
            #   error_prime += self.l1_lambda * np.sum(np.sign(layer.weights)) / N #add l1 regularization term
          error += self.loss(y_batch, output) #calculate loss (for display purposes only, not rly useful)
          # error += self.l2_lambda * l2_reg / (2*N) #add l2 regularization term
          # error += self.l1_lambda * l1_reg / (2*N) #add l1 regularization term
          #back prop
          error_prime += self.loss_prime(y_batch, output) #calculate error of output layer used in backprop
          for layer in reversed(self.layers):
            error_prime = layer.back_prop(error_prime, alpha) #backpropagate error through layers
        # calculate average error on all samples
        error /= num_it
        if val_data is not None and val_labels is not None:
          out2 = self.predict(val_data)
          print('epoch %d/%d   error=%f accuracy=%f' % (t+1, num_epochs, error, accuracy(out2, val_labels)))
        else:
          print('epoch %d/%d   loss=%f ' % (t+1, num_epochs, error))
          
net1 = Network()
net1.change_loss(cross_entropy_with_sm, cross_entropy_prime_with_sm)
net1.add(FCL(32*32*3, 256, l1_lambda=0.0, l2_lambda=0.0))
# net1.add(ActivationLayer(relu, relu_prime))
# net1.add(FCL(50, 50))
net1.add(ActivationLayer(relu, relu_prime))
net1.add(FCL(256, 10, l1_lambda=0.0, l2_lambda=0.0))
net1.fit(train_data_augmented, train_labels_augmented, num_epochs=10, alpha=0.001, batch_size = 64)

out = net1.predict(test_data)
test_labels = np.argmax(test_labels, axis=1)
print("test shapes: ", out.shape, np.array(test_labels).shape)
acc = accuracy(out,np.array(test_labels))
print("test accuracy: ", acc)
# sklearn.metrics.ConfusionMatrixDisplay.from_predictions(test_labels,out)
# plt.show()