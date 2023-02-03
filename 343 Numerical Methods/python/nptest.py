import numpy as np

def LU_Decomposition(A):
    """
    Input:
    A: (n,n) numpy array

    Outputs:
    L, U
    L: (n, n) numpy array
    U: (n, n) numpy array
    """
    L, U = np.eye(A.shape[0]), np.zeros(A.shape)
#     print("U \n",U,"\n L \n", L, "\n A \n", A)
    n = A.shape[1]
    for i in range(A.shape[0]):
        for j in range(A.shape[1]):
#             print("i,j: ", i,j)
#             if i>0:
#                 print("L[i,1:i], U[1:i,j] : \n", L[i,1:i],U[1:i,j])
#             else:
#                 print("i is 0, A[i,j]: ", A[i,j])
            U[i,j] = A[i,j] if i==0 else A[i,j]-L[i,0:i].dot(U[0:i,j])   
    for i in range(A.shape[0]):
        for j in range(A.shape[1]):
#             if j>0: 
#                 print("L[i,0:j], Utemp[0:j,j], Utemp[j,j] \n", L[i,0:j],U[0:j,j], U[j,j])
#             else:
#                 print("j is 0, A[i,j]/U[j,j]", A[i,j]/U[j,j])
            L[i,j] = A[i,j]/U[j,j] if j==0 else (A[i,j]-L[i,0:j].dot(U[0:j,j]))/U[j,j]
    U = np.triu(U)
    L = np.tril(L)
    return L, U
def ForwardSubstitution(L, b):
    """
    Inputs: 
    L: (n, n) numpy array
    b: (n,) numpy array

    Output: 
    y: (n,) numpy array
    """
    y = np.zeros(L.shape[0])

    ### BEGIN SOLUTION
    for i in range(y.size):
        y[i] = b[0]/L[0,0] if i==0 else (b[i]-L[i,0:i-2].dot(y[0:i-2]))/L[i,i]
    ### END SOLUTION
    return y
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
    
    ### BEGIN SOLUTION
    for i in range(n):
        x[i] = y[n-1]/U[n-1,n-1] if i==n-1 else (y[i]-U[i,i+1:n-1].dot(x[i+1:n-1]))/U[i,i]
    ### END SOLUTION
    return x
def LU_solver(A, b):
  """
  Inputs:
  A: (n,n) numpy array
  b: (n,) numpy array

  Output:
  x: (n,) numpy array
  """
  x = np.zeros(A.shape[0])

  ### BEGIN SOLUTION
  L,U = LU_Decomposition(A)
  y = ForwardSubstitution(L, b)
  x = BackwardSubstitution(U, y)
  ### END SOLUTION
  return x
#A = np.arange(25).reshape((5,5)).astype(float)
A =np.array([[2, -1, -2],
                           [-4, 6, 3],
                           [-4, -2, 8]])
L, U = LU_Decomposition(A)
print(L)
print(U)
print(L @ U)

# t = np.array([3,2,4,1,3])
# 
# print(L)
# print(U)

