class MiniBatchGD:
    def __init__(self, alpha=0.001, max_iters=1000, epsilon=1e-8, batch_size=32, record_history=False):
        self.alpha = alpha
        self.max_iters = max_iters
        self.record_history = record_history
        self.epsilon = epsilon
        self.batch_size = batch_size
        if record_history:
            self.w_history = []                 #to store the weight history for visualization
            
    def run(self, gradient_fn, x, y, w):
        grad = np.inf
        t = 1
        while np.linalg.norm(grad) > self.epsilon and t < self.max_iters:
            #randomly shuffle the data before each iteration
            indices = np.random.permutation(x.shape[0])
            x_shuffle, y_shuffle = x[indices], y[indices]
            for i in range(0, x.shape[0], self.batch_size):
                x_batch = x_shuffle[i:i+self.batch_size]
                y_batch = y_shuffle[i:i+self.batch_size]
                grad = gradient_fn(x_batch, y_batch, w)               # compute the gradient with present weight
                w = w - self.alpha * grad         # weight update step
                if self.record_history:
                    self.w_history.append(w)
            t += 1
        return w

class MiniLogisticRegression:
    def __init__(self, add_bias=True, alpha=0.1, epsilon=1e-4, max_iters=1e5, verbose=False, batch_size=32):
        self.add_bias = add_bias
        self.alpha = alpha
        self.epsilon = epsilon                        #to get the tolerance for the norm of gradients 
        self.max_iters = max_iters                    #maximum number of iteration of gradient descent
        self.verbose = verbose
        self.batch_size = batch_size
        self.gd = GradientDescent(alpha=self.alpha, max_iters=self.max_iters, epsilon=self.epsilon)

    def fit(self, x, y):
        if x.ndim == 1:
            x = x[:, None]
        if self.add_bias:
            N = x.shape[0]
            x = np.column_stack([x,np.ones(N)])
        N,D = x.shape
        self.w = np.zeros(D)
        g = np.inf 
        t = 0
        
        for i in range(0, N, self.batch_size):
            x_batch = x[i:i + self.batch_size]
            y_batch = y[i:i + self.batch_size]
            self.w = self.gd.run(gradient, x_batch, y_batch, self.w)
        
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
        yh = logistic(np.dot(x,self.w))            #predict output
        return yh

