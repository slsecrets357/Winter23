# [TODO] Rename this file to [your student ID].py

# DO NOT EDIT THESE IMPORT STATEMENTS!
import matplotlib.pyplot as plt   # plotting 
from scipy.linalg import lu_factor, lu_solve # scipy's LU solver
import numpy as np                # all of numpy, for now...            
del np.vander, np.linalg.inv      # ... except for two functions
##########################



# [TODO Deliverable 1] Perform an LU Decomposition of A (without pivoting)
def LU_decomposition(A):
    """
    Input:
    A: (n,n) numpy array

    Outputs:
    L, U
    L: (n, n) numpy array
    U: (n, n) numpy array
    """
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

# [TODO Deliverable 2] Solve a lower triangular system with forward substitution
def ForwardSubstitution(L, b):
    """
    Inputs: 
    L: (n, n) numpy array
    b: (n,) numpy array

    Output: 
    y: (n,) numpy array
    """
    n = L.shape[0]
    y = np.zeros(n)
    for i in range(n):
        y[i] = (b[i] - np.dot(L[i, :i], y[:i])) / L[i, i]
    return y

# [TODO Deliverable 3] Solve an upper triangular system with backward substitution
def BackwardSubstitution(U, y):
    """
    Perform back substitution to solve Ux=y where U is an upper triangular matrix

    Parameters: 
    U: (n, n) numpy array
    b: (n,) numpy array

    Returns: 
    x np.array(n): Solution vector
    """
    n = U.shape[0]
    x = np.zeros(n)
    for i in range(n-1, -1, -1):
        x[i] = (y[i] - np.dot(U[i, i+1:], x[i+1:])) / U[i, i]
    return x


# [TODO Deliverable 4] Solve the linear system Ax=b with a LU decomposition, forward and backward substitution.
def LU_solver(A, b):
    """
    Inputs:
    A: (n,n) numpy array
    b: (n,) numpy array

    Output:
    x: (n,) numpy array
    """
    L, U = LU_decomposition(A)
    y = ForwardSubstitution(L, b)
    x = BackwardSubstitution(U, y)
    return x



# [TODO Deliverable 5] Construct the degree-(n-1) Vandermonde polynomial regression matrix 
def GetVandermonde(t, n):
    """
    Inputs:
    t: (m,) numpy array
    n: scalar integer

    Output:
    A: (m, n) shaped numpy array
    """
    A = np.zeros((t.shape[0],n))

    ### BEGIN SOLUTION
    for i in range(n):
        A[:,i] = np.power(t,i)
    ### END SOLUTION
    return A

# [TODO Deliverable 6] Fit a degree-(n-1) polynomial to m data points
def PolynomialFit(data, n):
    """
    Input:
    data: (m,2) numpy array of data points (t_i, b_i)
    n: positive integer for an (n-1) degree polynomial

    Output:
    x: (n, ) numpy array of the polynomial coefficients for the terms t^0, t^1,... t^(n-1)
    """
    x = np.zeros((n,))

    ### BEGIN SOLUTION
    t = data[:, 0]
    b = data[:, 1]
    A = GetVandermonde(t, n)
    ATA = A.T.dot(A)
    ATb = A.T.dot(b)
    x = LU_solver(ATA, ATb)
    ### END SOLUTION
    return x



# [TODO: Deliverable 7]
def CreateBlurMatrix( kernel1Dx, width, height ):
    if (kernel1Dx.shape[0] % 2 == 0) or (kernel1Dx.shape[0] > width):
        print("ERROR in inputs.\n")
        return
    A = None
    ### BEGIN SOLUTION
    # get the matrix A
    A = get_A(kernel1Dx, (height, width))

    # reshape A into the desired shape
    blur_matrix = A.reshape((height, width, height, width))
    ### END SOLUTION
    return A
def get_A(k, img_shape):
    n = img_shape[0] * img_shape[1]  # number of pixels
    m = len(k)  # length of blur kernel
    A = np.zeros((n, n))
    for i in range(n):
        for j in range(m):
            if i-j >= 0 and i-j < n:
                A[i, i-j] = k[j]
    return A

# [TODO: Deliverable 8]
def BlurImage( blurMatrix, image ):
    """
    Given a blurMatrix and an image, this function should return a blurred image

    Parameters:
    blurMatrix: numpy array
    image: numpy array; image to blur

    Returns
    blurred_image: numpy array with same shape as `image`
    """

    blurred_img = np.zeros(image.shape)
    ### BEGIN SOLUTION
    # flatten the image into a column vector
    x = image.flatten()

    # apply the blur matrix to the image
    y = blurMatrix.dot(x)

    # reshape the output into the desired shape
    blurred_img = y.reshape(image.shape)

    ### END SOLUTION
    return blurred_img



# [TODO: Deliverable 9]
def DeblurImage( blurMatrix, blurred_image ):
    deblurred_img = np.zeros(blurred_image.shape[0])
    ### BEGIN SOLUTION
        # flatten the image into a column vector
    y = blurred_image.flatten()

    # compute the LU decomposition of the blur matrix
    LU,P = lu_factor(blurMatrix)

    # solve for x using the LU decomposition
    x = lu_solve((LU,P), y)

    # reshape the output into the desired shape
    deblurred_img = x.reshape(blurred_image.shape)

    ### END SOLUTION
    return deblurred_img



# Some example test routines for the deliverables. 
# Feel free to write and include your own tests here.
# Code in this main block will not count for credit, 
# but the collaboration and plagiarism policies still hold.
if __name__ == "__main__":
  
  print("\n\n[Deliverable 1] LU Factorization\n\n")
  test_matrix =  np.array([[2, -1, -2],
                           [-4, 6, 3],
                           [-4, -2, 8]])

  L, U = LU_decomposition(test_matrix)

  try:
    assert( np.isclose(test_matrix, L @ U).all() )
    assert( (np.tril(L) == L).all() )
    assert( (np.triu(U) == U).all() )
    print("LU decomposition passes one test.\n\n")
  except:
    raise Exception("LU decomposition fails one test.\n\n")

  

  print("[Deliverable 2] Forward Substitution\n\n")
  L = np.tril([[4,12,-16],[12,37,-43],[-16,-43,98]])
  b = np.array([1,2,3])         
  y = ForwardSubstitution(L, b)

  try:
    assert( np.isclose(L @ y, b).all() )
    print("Forward substitution passes one test.\n\n")
  except:
    raise Exception("Forward substitution failed one test.\n\n")



  print("[Deliverable 3] Backward Substitution\n\n")
  U = np.triu([[4.,12.,-16.],[12.,37.,-43.],[-16.,-43.,98.]])
  y = np.array([1.,2.,3.])        
  x = BackwardSubstitution(U, y)

  try:
    assert( np.isclose(U @ x, y).all() )
    print("Backward substitution passes one test.\n\n")
  except:
    raise Exception("Backward substitution failed one test.\n\n")



  print("[Deliverable 4] LU Solver\n\n")
  A =  np.array([[2, -1, -2],
              [-4, 6, 3],
              [-4, -2, 8]])
  x = np.array([1,2,3]) 
  b = A @ x
  lu_solved_x = LU_solver(A, b)

  try:
    assert(np.allclose(x, lu_solved_x))
    print("LU solver passes one test.\n\n")
  except:
    raise Exception("LU solver failed one test.\n\n")

  

  print("[Deliverable 5] Constructing Vandermonde Matrix")
  n = 5
  m = 10
  t = np.sort( np.random.randn(m) )
  A = GetVandermonde(t, n)
  # TODO: Write your own test, here; remember, you can add additional imports in the mainline



  print("[Deliverable 6a] Fully Constrained Polynomial Fitting")
  m = 10
  n = m
  t = np.sort( np.random.randn(m) )
  A = GetVandermonde(t, n)
  x = np.random.randn(n).astype('d')
  b = A @ x

  data = np.empty((m,2))  
  data[:,0] = t
  data[:,1] = b

  fit_x = PolynomialFit(data,n)
  fit_b = A @ fit_x
  
  _, plots = plt.subplots(2)
  plots[0].scatter(t, b, color='blue', label='Data')
  plots[0].plot(t, fit_b, color='orange', label='Your curve')
  plots[0].set_title(f'Fully Constrained Polynomial Fit', size=8)
  plots[0].set_ylabel("b -- polynomial points")
  plots[0].legend()
  
  print("[Deliverable 6b] Overdetermined Polynomial Fitting")
  m = 20
  n = 5
  t = np.sort( np.random.randn(m) )
  A = GetVandermonde(t, n)
  assert A.shape[0] > A.shape[1]
  x = np.random.randn(n)
  b = np.random.normal(A @ x, scale=np.ones((m,)))
  
  data = np.empty((m,2))  
  data[:,0] = t
  data[:,1] = b

  fit_x = PolynomialFit(data,n)
  fit_b = A @ fit_x

  plots[1].scatter(t, b, color='blue', label='Data')
  plots[1].plot(t, fit_b, color='orange', label='Your curve')
  plots[1].set_title(f'Overdetermined Least-squares Polynomial Fit', size=8)
  plots[1].set_xlabel("t -- independent polynomial variable")
  plots[1].set_ylabel("b -- polynomial points")
  plots[1].legend()
  
  plt.show() # this is a blocking call; kill the plotting window to continue execution



  print("[Deliverable 7 - 9] Blurring and deblurring")
  # Load test data from file
  image, blurred_image, kernel_1d = np.empty((100,100)), np.empty((100,100)), np.empty((21,))
  with open('Q3-test-data.npy', 'rb') as f:
    image = np.load(f)
    blurred_image = np.load(f)
    kernel_1d = np.load(f)

  A = CreateBlurMatrix( kernel_1d, image.shape[0], image.shape[1] )
  
  # Display the test images we provide, as well as 
  # your (to-be-completed) blurred and deblurred images
  cmap = plt.get_cmap('gray')
  _, plots = plt.subplots(2, 2)
  plt.setp(plots, xticks=[], yticks=[])
  
  plots[0,0].set_title('original image - from file', size=8)
  plots[0,0].imshow(image, cmap, vmin=0, vmax=1)
  
  plots[0,1].set_title('blurred image - from file', size=8)
  plots[0,1].imshow(blurred_image, cmap, vmin=0, vmax=1)

  plots[1,1].set_title('blurred image - computed', size=8)
  plots[1,1].imshow( BlurImage(A, image), cmap, vmin=0, vmax=1)

  plots[1,0].set_title('deblurred image - computed', size=8)
  plots[1,0].imshow( DeblurImage(A, blurred_image), cmap, vmin=0, vmax=1)
  
  plt.show() # this is a blocking call; kill the plotting window to continue execution
  