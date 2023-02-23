import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
import pickle
from numpy import newaxis
import sklearn
import math

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
print(test_data1.shape)
test_labels1 = test_batch[b'labels']
print(len(test_labels1))

train_data1 = np.concatenate((data_batches[0][b'data'], data_batches[1][b'data'], data_batches[2][b'data'], data_batches[3][b'data'], data_batches[4][b'data']), axis=0)
print(train_data1.shape)
train_labels1 = np.concatenate((data_batches[0][b'labels'], data_batches[1][b'labels'], data_batches[2][b'labels'], data_batches[3][b'labels'], data_batches[4][b'labels']), axis=0)
print(train_labels1.shape)

#one hot encode
def one_hot(y, num_classes):
  return np.eye(num_classes)[y]
num_classes = 10
train_labels = one_hot(train_labels1, num_classes)
#train_labels = np.array(train_labels1)
print(train_labels.shape)
test_labels = one_hot(test_labels1, num_classes)
#test_labels = np.array(test_labels1)
print(test_labels.shape)

#normalize
train_data = train_data1/255.
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
  def __init__(self, input_size, output_size):
    self.weights = np.random.rand(input_size, output_size) - 0.5
    self.bias = np.random.rand(1, output_size) - 0.5
  def forward_prop(self, input_data):
    self.input = input_data
    self.output = np.dot(self.input, self.weights) + self.bias
    return self.output
  def back_prop(self, output_error, alpha):
    input_error = np.dot(output_error, self.weights.T)#/output_error.shape[1]
    weight_gradient = np.dot(self.input.T, output_error)/output_error.shape[0]
    bias_gradient = np.sum(output_error)/output_error.shape[0]
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
    return self.activation_prime(self.input) * output_error

# activation functions and their derivatives
def tanh(x):
    return np.tanh(x)
def tanh_prime(x):
    return 1-np.tanh(x)**2
def relu(x):
    # print("relu: ", x.shape, "\n", x)
    # a = np.maximum(x,0)
    # print("after: \n", a.shape, "\n", a )
    return np.maximum(x,0)
def relu_prime(x):
    return x>0
def sigmoid(x):
    return 1 /(1 + np.exp(-x))
def sigmoid_prime(x):
    s = sigmoid(x)
    return s*(1 - s)
def softmax(z):
    # expz = np.exp(z)
    expz = np.exp(z - np.max(z, axis=1, keepdims=True)) #seems more stable numerically .-.
    # print("softmax z: ", z.shape, "\n", z)
    # a = expz/np.sum(expz,axis=1,keepdims=True)
    # print("after: \n", a.shape, "\n", a )
    y = expz/np.sum(expz,axis=1,keepdims=True)
    if not np.allclose(np.sum(y, axis=1), 1):
        raise ValueError("Probabilities do not sum to 1 along class axis.")
    y = np.clip(y, 1e-12, 1 - 1e-12)
    return y
def softmax_prime(z):
    softm = softmax(z)
    # print("softmaxprime z: ", z.shape, "\n", z)
    # a = softm*(1-softm)
    # print("after: \n", a.shape, "\n", a )
    y = softm*(1-softm)
    return y

#error functions
def mse(y_true, y_pred):
    # print("mse : ypred ", y_pred.shape, "\n", y_pred)
    # print("mse : ytrue ", y_true.shape, "\n", y_true)
    # a = (np.power(y_true-y_pred, 2)) 
    # print("after: \n", a.shape, "\n", a )
    return np.mean(np.power(y_true-y_pred, 2)) 
def mse_prime(y_true, y_pred):
    # print("mse prime : ypred ", y_pred.shape, "\n", y_pred)
    # print("mse prime: ytrue ", y_true.shape, "\n", y_true)
    # a = (y_pred-y_true) 
    # print("after: \n", a.shape, "\n", a )
    return 2*(y_pred-y_true)

# def cross_entropy(y_true, y_pred):
#     epsilon = 1e-12  
#     y_pred = np.clip(y_pred, epsilon, 1.0 - epsilon)
#     return -np.mean(np.sum(y_true * np.log(y_pred), axis=1)) 
# def cross_entropy_prime(y_true, y_pred, epsilon = 1e-12):
#     y_pred = np.clip(y_pred, epsilon, 1.0 - epsilon) #add small value to avoid division by 0
#     return -y_true/y_pred
def cross_entropy_with_sm(y_true,y_pred): #with softmax
    '''
    y_true is one-hot encoded vector
    calculates cross entropy loss by first applying softmax to y_pred
    then calculates cross entropy loss of y_true and y_pred
    '''
    # epsilon = 1e-12  
    # y_pred = np.clip(y_pred, epsilon, 1.0 - epsilon)
    # p = softmax(y_pred)
    # return -np.mean(np.sum(y_true * np.log(p), axis=1)) 
    p = softmax(y_pred)
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
# def cross_entropy(y,X):
#     """
#     X is the output from fully connected layer (num_examples x num_classes)
#     y is labels (num_examples x 1)
#     	Note that y is not one-hot encoded vector. 
#     	It can be computed as y.argmax(axis=1) from one-hot encoded vectors of labels if required.
#     """
#     m = y.shape[0]
#     p = softmax(X)
#     # We use multidimensional array indexing to extract 
#     # softmax probability of the correct label for each sample.
#     # Refer to https://docs.scipy.org/doc/numpy/user/basics.indexing.html#indexing-multi-dimensional-arrays for understanding multidimensional array indexing.
#     log_likelihood = -np.log(p[range(m),y])
#     loss = np.sum(log_likelihood) / m
#     return loss
# def cross_entropy_prime(y,X):
#     """
#     X is the output from fully connected layer (num_examples x num_classes)
#     y is labels (num_examples x 1)
#     	Note that y is not one-hot encoded vector. 
#     	It can be computed as y.argmax(axis=1) from one-hot encoded vectors of labels if required.
#     """
#     m = y.shape[0]
#     grad = softmax(X)
#     grad[range(m),y] -= 1
#     grad = grad/m
#     return grad
#helper functions
#compares prediction to actual labels and computes accuracy
def accuracy(prediction, actual):
    if actual.ndim != 1:
      actual = np.argmax(actual, axis=1)
    print("calculating accuracy. prediction, actual: ", prediction.shape, actual.shape)
    size = prediction.shape[0]
    return np.sum(prediction == actual)/size

class Network:
    def __init__(self, epsilon=1e-4, loss = mse, loss_prime = mse_prime):
        self.layers = []
        self.loss = loss
        self.loss_prime = loss_prime
        self.epsilon = epsilon
        np.random.seed(0)

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

    #optimizing using minibatch gradient descent
    def fit(self, x, y, num_epochs, alpha, optimizer=None, batch_size = 32, val_data = None, val_labels=None):
      num_it = math.ceil(x.shape[0]/ batch_size)
      print(f"optimizing with gradient descent, batch size = {batch_size}, num_it={num_it}")
      indices = np.random.permutation(x.shape[0])
      x1, y1 = x[indices], y[indices]
      if val_data is None or val_labels is None:
        print("validation data not given. using 10% of train data as val")
        val_data, val_labels = x1[0:int(x1.shape[0]/10)], y1[0:int(y1.shape[0]/10)]
      for t in range(num_epochs):
        error = 0
        indices = np.random.permutation(x.shape[0])
        x_shuffle, y_shuffle = x1[indices], y1[indices]
        for i in range(0, x.shape[0], batch_size):
          x_batch = x_shuffle[i:i+batch_size]
          y_batch = y_shuffle[i:i+batch_size]
          output = x_batch
          N,D = x1.shape
          # forward propagation
          for layer in self.layers:
            output = layer.forward_prop(output)
          error += self.loss(y_batch, output)
          #back prop
          error_prime = self.loss_prime(y_batch, output)
#           print("error prime : ypred ", output.shape, "\n", output)
#           print("error prime: ytrue ", y_batch.shape, "\n", y_batch)
#           print("after: \n", error_prime.shape, "\n", error_prime)
          for layer in reversed(self.layers):
            error_prime = layer.back_prop(error_prime, alpha)
        # calculate average error on all samples
        error /= num_it
        if val_data is not None and val_labels is not None:
          out2 = self.predict(val_data)
          print('epoch %d/%d   error=%f accuracy=%f' % (t+1, num_epochs, error, accuracy(out2, val_labels)))
        else:
          print('epoch %d/%d   loss=%f ' % (t+1, num_epochs, error))
          
net1 = Network()
net1.change_loss(cross_entropy_with_sm, cross_entropy_prime_with_sm)
net1.add(FCL(32*32*3, 10))
#net1.add(ActivationLayer(relu, relu_prime))
# net1.add(FCL(50, 50))
# net1.add(ActivationLayer(relu, relu_prime))
#net1.add(FCL(50, 10))
#net1.add(ActivationLayer(softmax, softmax_prime))
net1.fit(train_data, train_labels, num_epochs=5, alpha=0.001, batch_size = 64)

out = net1.predict(test_data)
test_labels = np.argmax(test_labels, axis=1)
print("test shapes: ", out.shape, np.array(test_labels).shape)
acc = accuracy(out,np.array(test_labels))
print("test accuracy: ", acc)
sklearn.metrics.ConfusionMatrixDisplay.from_predictions(test_labels,out)
plt.show()