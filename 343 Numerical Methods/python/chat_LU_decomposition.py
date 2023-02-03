import numpy as np
import matplotlib.pyplot as plt   # plotting 
from scipy.linalg import lu_factor, lu_solve

def LU_decomposition(matrix):
    # Convert the input matrix to a NumPy array
    A = np.array(matrix)
    
    # Check that the input matrix is square
    n = A.shape[0]
    if A.shape[1] != n:
        raise ValueError('Input matrix must be square')
    
    # Initialize L and U as the identity matrix
    L = np.eye(n)
    U = np.eye(n)
    
    # Perform the LU decomposition
    for k in range(n):
        # Compute the entries of the k-th column of U
        for j in range(k, n):
            U[k, j] = A[k, j] - np.dot(L[k, :k], U[:k, j])
        
        # Compute the entries of the k-th row of L
        for i in range(k + 1, n):
            L[i, k] = (A[i, k] - np.dot(L[i, :k], U[:k, k])) / U[k, k]
    
    return L, U
def LU_decomposition2(A):
    n = A.shape[0]
    L = np.identity(n)
    U = np.copy(A)
    for j in range(n-1):
        for i in range(j+1, n):
            if U[i, j] != 0:
                L[i, j] = U[i, j] / U[j, j]
                U[i, j:] = U[i, j:] - L[i, j] * U[j, j:]
                U[i, j] = 0
    return L, U
def forward_substitution(L, b):
    n = L.shape[0]
    y = np.zeros(n)
    for i in range(n):
        y[i] = (b[i] - np.dot(L[i, :i], y[:i])) / L[i, i]
    return y
def backward_substitution(U, y):
    n = U.shape[0]
    x = np.zeros(n)
    for i in range(n-1, -1, -1):
        x[i] = (y[i] - np.dot(U[i, i+1:], x[i+1:])) / U[i, i]
    return x
def LU_solver(A, b):
    L, U = LU_decomposition(A)
    y = forward_substitution(L, b)
    x = backward_substitution(U, y)
    return x
def GetVandermonde(t, n):
    m = len(t)
    A = np.ones((m, n))
    for i in range(1, n):
        A[:, i] = t**i
    return A
def PolynomialFit(data, n):
    t = data[:, 0]
    b = data[:, 1]
    A = GetVandermonde(t, n)
    ATA = A.T.dot(A)
    ATb = A.T.dot(b)
    x = LU_solver(ATA, ATb)
    return x
def CreateBlurMatrix(k, width, height):
    # get the matrix A
    A = get_A(k, (height, width))
    
    # reshape A into the desired shape
    blur_matrix = A.reshape((height, width, height, width))
    
    return blur_matrix
def get_A(k, img_shape):
    n = img_shape[0] * img_shape[1]  # number of pixels
    m = len(k)  # length of blur kernel
    A = np.zeros((n, n))
    for i in range(n):
        for j in range(m):
            if i-j >= 0 and i-j < n:
                A[i, i-j] = k[j]
    return A
def BlurImage(blur_matrix, img):
    # flatten the image into a column vector
    x = img.flatten()
    
    # apply the blur matrix to the image
    y = blur_matrix.dot(x)
    
    # reshape the output into the desired shape
    blurred_img = y.reshape(img.shape)
    
    return blurred_img
def DeblurImage(blur_matrix, img):
    # flatten the image into a column vector
    y = img.flatten()
    
    # compute the LU decomposition of the blur matrix
    P, L, U = lu_factor(blur_matrix)
    
    # solve for x using the LU decomposition
    x = lu_solve((P, L, U), y)
    
    # reshape the output into the desired shape
    deblurred_img = x.reshape(img.shape)
    
    return deblurred_img

# Example usage
A = np.array([[2, 1, 3],
     [4, 1, 2],
     [0, 3, 1]])
A2 = np.array([[2, 1, 1], [4, 3, 3], [8, 7, 9]])
L, U = LU_decomposition(A)
L2, U2 = LU_decomposition2(A)
b = np.array([1, 2, 3])
y = forward_substitution(L, b)
x = backward_substitution(U, y)

print('L =')
print(L)
print('U =')
print(U)
print("y:")
print(y)
print("L*y:")
print(L.dot(y))
print("x:")
print(x)

# check that U*x equals y
print("U*x:")
print(U.dot(x))

data = np.array([[1, 2], [2, 4], [3, 6], [4, 8], [5, 10]])
n = 3

# fit the polynomial to the data
x = PolynomialFit(data, n)

print("Coefficients:")
print(x)

k = np.array([1, 2, 1])
width = 5
height = 5

# create the blur matrix
blur_matrix = CreateBlurMatrix(k, width, height)

