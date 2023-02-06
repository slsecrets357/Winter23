import pandas as pd
import numpy as np
import scipy
import matplotlib as mpl
import matplotlib.pyplot as plt
import random
#optimizers
#1) gradient descent class
class GradientDescent: 
    def __init__(self, learning_rate=.001, max_iters=1e3, epsilon=1e-8, record_history=False):
        self.learning_rate = learning_rate
        self.max_iters = max_iters
        self.record_history = record_history
        self.epsilon = epsilon
        if record_history:
            self.w_history = []                 #to store the weight history for visualization
            
    def run(self, gradient_fn, x, y, w, activation=None, batch_size = 0):
        grad = np.inf
        t = 1
        if batch_size<1:
            print("performing batch gradient descent...")
        elif batch_size ==1:
            print("stochastic gradient descent...")
        else:
            print(f"mini-batch gradient descent, batch size={batch_size}")
        while np.linalg.norm(grad) > self.epsilon and t < self.max_iters:
            if t%10==0:
              print("t: ",t,", grad: ", np.linalg.norm(grad))
             
            if batch_size<1:  #batch gd 
                N,D = x.shape
                if activation is None:
                    yh = np.dot(x,w)
                else:
                    yh = activation(np.dot(x, w))    # predictions  size N
                grad = gradient_fn(x, y, yh)               # compute the gradient with present weight
                w = w - self.learning_rate * grad         # weight update step
                if self.record_history:
                    self.w_history.append(w)
            else: #mini-batch gd
                #randomly shuffle the data before each iteration
                indices = np.random.permutation(x.shape[0])
                x_shuffle, y_shuffle = x[indices], y[indices]
                for i in range(0, x.shape[0], batch_size):
                  x_batch = x_shuffle[i:i+batch_size]
                  y_batch = y_shuffle[i:i+batch_size]
                  N,D = x.shape
                  if activation is None:
                      yh = np.dot(x,w)
                  else:
                      yh = activation(np.dot(x, w))    # predictions  size N
                  grad = np.dot(x.T, yh - y)/N        # divide by N because cost is mean over N points
                  w = w - self.learning_rate * grad         # weight update step
                  if self.record_history:
                    self.w_history.append(w)
            t += 1
        return w

class LinearRegression:
    def __init__(self, add_bias=True):
        self.add_bias = add_bias
        pass
    
    def fit(self, x, y, optimizer=None):
        print(f"fit: x: {x.shape}, y: {y.shape}")
        if x.ndim == 1:
            x = x[:, None]                         #add a dimension for the features
        if self.add_bias:
            N = x.shape[0]
            x = np.column_stack([x,np.ones(N)])   #add bias by adding a constant feature of value 1

        if optimizer == None: #analytical linear reg
          self.w = np.linalg.lstsq(x, y)[0]          #return w for the least square difference
        else: #using optimizer (gradient descent)
          N,D = x.shape
          def gradient(x, y, yh):                          # define the gradient function
              N, D = x.shape
              grad = .5*np.dot(x.T, yh - y)/N
              return grad
          w0 = np.zeros((D,y.shape[1]))                                # initialize the weights to 0
          self.w = optimizer.run(gradient, x, y, w0)      # run the optimizer to get the optimal weights
        return self
    
    def predict(self, x):
        if self.add_bias:
            x = np.column_stack([x,np.ones(N)])
        yh = np.dot(x,self.w)                            #predict the y values
        return yh

def cost_fn(x, y, w):
    N, D = x.shape                                                       
    z = np.dot(x, w)
    J = np.mean(y * np.log1p(np.exp(-z)) + (1-y) * np.log1p(np.exp(z)))  #log1p calculates log(1+x) to remove floating point inaccuracies 
    return J
def gradient(x, y, yh):
    N,D = x.shape
    grad = np.dot(x.T, yh - y)/N        # divide by N because cost is mean over N points
    return grad                         # size D
def sigmoid(y):
    return 1. / (1 + np.exp(-y))
class LogisticRegression:
    
    def __init__(self, add_bias=True, learning_rate=.1, epsilon=1e-4, max_iters=1e5, verbose=False):
        self.add_bias = add_bias
        self.learning_rate = learning_rate
        self.learning_rate_batch = learning_rate/100
        self.epsilon = epsilon                        #to get the tolerance for the norm of gradients 
        self.max_iters = max_iters                    #maximum number of iteration of gradient descent
        self.verbose = verbose
  
    def fit(self, x, y, optimizer=None, bs = 0):
        if x.ndim == 1:
            x = x[:, None]
        if self.add_bias:
            N = x.shape[0]
            x = np.column_stack([x,np.ones(N)])
        N,D = x.shape
        self.w = np.zeros(D)
        g = np.inf 
        t = 0
        #gradient descent
        if optimizer is not None: #use optimizer class
          self.w = optimizer.run(gradient, x, y, self.w, activation = sigmoid, batch_size = bs)
        elif bs>=1: #mini-batch gradient descent
          while np.linalg.norm(g) > self.epsilon and t < self.max_iters:
            for i in range(0,N, bs):
              #create mini batches
              x_batch = x[i:i+bs]
              y_batch = y[i:i+bs]
              g = self.gradient(x_batch, y_batch)
              self.w = self.w - self.learning_rate_batch * g 
            t += 1    
        else: #batch gd     
          while np.linalg.norm(g) > self.epsilon and t < self.max_iters:
              g = self.gradient(x, y)
              self.w = self.w - self.learning_rate * g 
              t += 1
        
        if self.verbose:
            print(f'terminated after {t} iterations, with norm of the gradient equal to {np.linalg.norm(g)}')
            print(f'the weight found: {self.w}')
        return self
    
    def predict(self, x):
        if x.ndim == 1:
            x = x[:, None]
        Nt = x.shape[0]
        if self.add_bias:
            x = np.column_stack([x,np.ones(Nt)])
        yh = sigmoid(np.dot(x,self.w))            #predict output
        return yh

LogisticRegression.gradient = gradient             #initialize the gradient method of the LogisticRegression class with gradient function

data1 = 'ENB2012_data.xlsx'
df1 = pd.read_excel(data1)
missing_mask1 = df1.eq('?').any(1)
df1 = df1[~missing_mask1] #remove missing or malformed features
data2 = 'Qualitative_Bankruptcy/Qualitative_Bankruptcy.data.txt'
df2 = pd.read_csv(data2, delimiter=',')
missing_mask = df2.eq('?').any(1)
df2 = df2[~missing_mask] #remove missing or malformed features
df2.replace({"P": 1, "N": 0, "A":0.5},inplace=True)
# replace 'B' with 1 and 'NB' with 0 in df2
df2.replace({"B": 1, "NB": 0},inplace=True)

indices = list(range(df1.shape[0]))
random.shuffle(indices) #create random indices
split_index = int(0.8*df1.shape[0]) 
X_train = df1.iloc[indices[:split_index],:-2].values #80% for training
X_test = df1.iloc[indices[split_index:],:-2].values #20% for testing
y_train = df1.iloc[indices[:split_index],-2:].values 
y_test = df1.iloc[indices[split_index:],-2:].values
print(X_train.shape, X_test.shape, y_train.shape, y_test.shape, type(X_train), type(y_train))

#process data2
indices = list(range(df2.shape[0]))
random.shuffle(indices)
split_index = int(0.8*df2.shape[0])
X_train2 = df2.iloc[indices[:split_index],:-1].values
X_test2 = df2.iloc[indices[split_index:],:-1].values
y_train2 = df2.iloc[indices[:split_index],-1:].values
y_test2 = df2.iloc[indices[split_index:],-1:].values
y_train2 = y_train2.reshape(-1,)
print(X_train2.shape, X_test2.shape, y_train2.shape, y_test2.shape)

# lin_reg = LinearRegression(add_bias=False)
# optimizer = BatchGradientDescent()
# lin_reg.fit(X_train, y_train, optimizer)
# print("w: ",lin_reg.w.shape)
# N = X_test.shape[0]
# yh = lin_reg.predict(X_test)
# loss = np.mean(y_test * np.log1p(yh) + (1-y_test) * np.log1p(yh))
# loss

logistic_reg = LogisticRegression()
optimizer = GradientDescent(learning_rate=.01, max_iters=150, record_history=True)
logistic_reg.fit(X_train2, y_train2, optimizer, bs = 32)
yh = logistic_reg.predict(X_test2)
yh = yh[:, None]
yh.shape
J = np.mean(y_test2 * np.log1p(yh) + (1-y_test2) * np.log1p(yh))
J

