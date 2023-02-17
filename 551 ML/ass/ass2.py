import numpy as np
import matplotlib.pyplot as plt
import pickle
import os
import sys
import time
import random
import math

def unpickle(file):
    import pickle
    with open(file, 'rb') as fo:
        dict = pickle.load(fo, encoding='bytes')
    return dict

file = 'cifar/data_batch_2'
a = unpickle(file)

class FullyConnectedLayer():
    """ A fully connected layer in a neural network. """
    def __init__(self, n_input, n_output, activation_fn=None, weights=None, bias=None):
        self.n_input = n_input
        self.n_output = n_output
        self.activation_fn = activation_fn
        self.weights = weights
        self.bias = bias
        self.input = None
        self.output = None
        self.delta = None
        self.weight_grad = None
        self.bias_grad = None
        self.init_weights()
    
    def forward_propagation(self, input):
        """ Performs forward propagation of the input signal.
        :param input: Input signal
        :return: Output value
        """
        self.input = input
        self.output = self.activation_fn(np.dot(input, self.weights) + self.bias)
        return self.output
    
    def backward_propagation(self, output_error, learning_rate):
        """ Performs backward propagation of the error signal.
        :param output_error: Gradient of the cost function with respect to the output value
        :param learning_rate: Learning rate
        :return: Gradient of the cost function with respect to the input value
        """
        self.delta = output_error * self.activation_fn(self.input, derivative=True)
        self.weight_grad = np.dot(self.input.T, self.delta)
        self.bias_grad = np.sum(self.delta, axis=0, keepdims=True)
        self.weights -= learning_rate * self.weight_grad
        self.bias -= learning_rate * self.bias_grad
        return np.dot(self.delta, self.weights.T)
    
    def init_weights(self):
        """ Initializes weights with random values. """
        limit = 1 / math.sqrt(self.n_input)
        self.weights = np.random.uniform(-limit, limit, (self.n_input, self.n_output))
        self.bias = np.zeros((1, self.n_output))

def relu(x, derivative=False):
    """ Rectified Linear Unit activation function. """
    if derivative:
        return 1. * (x > 0)
    return x * (x > 0)
def softmax(x, derivative=False):
    """ Softmax activation function. """
    if derivative:
        e_x = np.exp(x - np.max(x))
        softm = e_x / e_x.sum(axis=0)
        return softm * (1 - softm)
    e_x = np.exp(x - np.max(x))
    return e_x / e_x.sum(axis=0)
class SGD():

    def __init__(self, learning_rate):
        self.learning_rate = learning_rate

    def update(self, layer):
        """ Updates the weights and biases of the layer.
        :param layer: Layer to be updated
        """
        layer.weights -= self.learning_rate * layer.weight_grad
        layer.bias -= self.learning_rate * layer.bias_grad
    
    def update_weights(self, layer):
        """ Updates the weights of the layer.
        :param layer: Layer to be updated
        """
        layer.weights -= self.learning_rate * layer.weight_grad
    
    def update_bias(self, layer):
        """ Updates the bias of the layer.
        :param layer: Layer to be updated
        """
        layer.bias -= self.learning_rate * layer.bias_grad
    
    def update_learning_rate(self, learning_rate):
        """ Updates the learning rate.
        :param learning_rate: New learning rate
        """
        self.learning_rate = learning_rate
    
    def get_learning_rate(self):
        """ Returns the learning rate.
        :return: Learning rate
        """
        return self.learning_rate
    
    def update_all(self, layers):
        """ Updates the weights and biases of all layers.
        :param layers: Layers to be updated
        """
        for layer in layers:
            self.update(layer)
    
class NeuralNetwork():

    def __init__(self, layers, loss_fn):
        self.layers = layers
        self.loss_fn = loss_fn
        self.loss = None
        self.output = None
    
    def forward_propagation(self, input):
        """ Performs forward propagation of the input signal.
        :param input: Input signal
        :return: Output value
        """
        output = input
        for layer in self.layers:
            output = layer.forward_propagation(output)
        self.output = output
        return output
    
    def backward_propagation(self, target, learning_rate):
        """ Performs backward propagation of the error signal.
        :param target: Target value
        :param learning_rate: Learning rate
        :return: Gradient of the cost function with respect to the input value
        """
        loss = self.loss_fn(self.output, target)
        self.loss = loss
        error = loss
        for layer in reversed(self.layers):
            error = layer.backward_propagation(error, learning_rate)
        return error
    
    def add_layer(self, layer):
        """ Adds a layer to the neural network.
        :param layer: Layer to be added
        """
        self.layers.append(layer)

    def predict(self, input):
        """ Predicts the output for the given input.
        :param input: Input signal
        :return: Output value
        """
        output = input
        for layer in self.layers:
            output = layer.forward_propagation(output)
        return output
    
    def shuffle_data(self, X, y):
        """ Shuffles the data.
        :param X: Training data
        :param y: Training labels
        :return: Shuffled data and labels
        """
        indices = np.arange(len(X))
        np.random.shuffle(indices)
        return X[indices], y[indices]
    
    def train(self, X, y, learning_rate, epochs, optimizer, batch_size=32):
        """ Trains the neural network.
        :param X: Training data
        :param y: Training labels
        :param learning_rate: Learning rate
        :param epochs: Number of epochs
        :param optimizer: Optimizer
        :param batch_size: Batch size
        """
        #shuffle the data
        X, y = self.shuffle_data(X, y)
        #split the data into batches
        batches = []
        for i in range(0, len(X), batch_size):
            batches.append((X[i:i+batch_size], y[i:i+batch_size]))
        #train the network for the given number of epochs
        for i in range(epochs):
            for X_batch, y_batch in batches:
                self.forward_propagation(X_batch)
                self.backward_propagation(y_batch, learning_rate)
                optimizer.update_all(self.layers)
            print('Epoch: %d, Loss: %.3f' % (i+1, self.loss))
    
    def evaluate(self, X, y):
        """ Evaluates the neural network.
        :param X: Test data
        :param y: Test labels
        :return: Accuracy
        """
        correct = 0
        for i in range(len(X)):
            prediction = self.predict(X[i])
            if np.argmax(prediction) == np.argmax(y[i]):
                correct += 1
        return correct / len(X)
def cross_entropy(y_pred, y_true):
    """ Cross entropy loss function. """
    n_samples = y_pred.shape[0]
    logp = -np.log(y_pred[range(n_samples), y_true.argmax(axis=1)])
    loss = np.sum(logp) / n_samples
    return loss
def cross_entropy_derivative(y_pred, y_true):
    """ Derivative of the cross entropy loss function. """
    n_samples = y_pred.shape[0]
    grad = y_pred
    grad[range(n_samples), y_true.argmax(axis=1)] -= 1
    grad /= n_samples
    return grad
def accuracy(y_pred, y_true):
    """ Accuracy metric. """
    correct = 0
    for i in range(len(y_pred)):
        if np.argmax(y_pred[i]) == np.argmax(y_true[i]):
            correct += 1
    return correct / len(y_pred)
def mean_squared_error(y_pred, y_true):
    """ Mean squared error loss function. """
    return np.mean(np.power(y_true - y_pred, 2))
def mean_squared_error_derivative(y_pred, y_true):
    """ Derivative of the mean squared error loss function. """
    return 2 * (y_pred - y_true) / y_true.size
def one_hot_encode(y):
    """ One-hot encodes the labels.
    :param y: Labels
    :return: One-hot encoded labels
    """
    n_values = np.max(y) + 1
    return np.eye(n_values)[y]

data_batches = []
for i in range(1,6):
    file = f'cifar/data_batch_{i}'
    data_batches.append(unpickle(file))
test_batch = unpickle('cifar/test_batch')
test_data1 = test_batch[b'data']
test_labels1 = test_batch[b'labels']
train_data1 = np.concatenate((data_batches[0][b'data'], data_batches[1][b'data'], data_batches[2][b'data'], data_batches[3][b'data'], data_batches[4][b'data']))
train_labels1 = np.concatenate((data_batches[0][b'labels'], data_batches[1][b'labels'], data_batches[2][b'labels'], data_batches[3][b'labels'], data_batches[4][b'labels']))

train_data = train_data1/255
test_data = test_data1/255

layers = [
    FullyConnectedLayer(3072, 100, activation_fn=relu),
    FullyConnectedLayer(100, 10, activation_fn=softmax)
]

nn = NeuralNetwork(layers, cross_entropy)

nn.train(train_data, one_hot_encode(train_labels1), learning_rate=0.01, epochs=10, optimizer=SGD(), batch_size=32)

print('Accuracy: %.3f' % nn.evaluate(test_data, one_hot_encode(test_labels1)))


