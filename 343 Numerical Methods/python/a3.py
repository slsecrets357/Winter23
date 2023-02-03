# [TODO] Rename this file to [your student ID].py

# DO NOT EDIT THESE IMPORT STATEMENTS!
import matplotlib.pyplot as plt   # plotting 
import numpy as np                # all of numpy
##########################


def CreateBlurMatrix(kernel2D, width, height):
  # Initialize the blur matrix with zeros
  blur_matrix = np.zeros((width*height, width*height))

  # Get the size of the kernel
  k_size = kernel2D.shape[0]

  # Iterate over the pixels of the image
  for i in range(height):
    for j in range(width):
      # Calculate the index of the current pixel in the blur matrix
      pixel_idx = i * width + j

      # Iterate over the kernel elements
      for m in range(-k_size//2, k_size//2+1):
        for n in range(-k_size//2, k_size//2+1):
          # Calculate the row and column indices of the corresponding element in the blur matrix
          row_idx = i + m + k_size//2
          col_idx = j + n + k_size//2

          # Check if the indices are within the bounds of the image
          if row_idx >= 0 and row_idx < height and col_idx >= 0 and col_idx < width:
            # Calculate the index of the corresponding element in the blur matrix
            kernel_idx = (m + k_size//2) * k_size + (n + k_size//2)
            blur_matrix[pixel_idx, row_idx*width + col_idx] = kernel2D[kernel_idx]



# [TODO: Deliverable 2] Output a discrete k x k 2D Gaussian blur kernel
def Gaussian2D( k, sigma ):
  kernel2D = None
  # Initialize the kernel with zeros
  kernel2D = np.zeros((k, k))

  # Calculate the center of the kernel
  center = k // 2

  # Iterate over the kernel elements
  for i in range(k):
    for j in range(k):
      # Calculate the distance from the center
      dx = i - center
      dy = j - center
      distance = np.sqrt(dx**2 + dy**2)

      # Calculate the Gaussian value at the current position
      kernel2D[i, j] = np.exp(-distance**2 / (2 * sigma**2)) / (2 * np.pi * sigma**2)

  return kernel2D



# [TODO: Deliverable 3] Compute and return the deblurred image with a Tikhonov-regularized linear system solve
def DeblurImage( blurMatrix, blurred_image, lambda_ = 0.0 ):
  deblurred_img = None
    # Flatten the blurred image
  b = blurred_image.flatten()

  # Construct the augmented/regularized linear system of equations
  A = blurMatrix + lambda_ * np.eye(blurMatrix.shape[0])
  x = np.linalg.solve(A, b)

  # Reshape the solution to get the deblurred image
  deblurred_img = x.reshape(blurred_image.shape)

  return deblurred_img



# [TODO: Deliverable 4] Compute and return your highest quality deblurred image using whatever linear system formulation and solver you like
def DeblurImageCompetition( blurMatrix, blurred_image):
  deblurred_img = None
  ### BEGIN SOLUTION
  ### END SOLUTION
  return deblurred_img



# [TODO: Deliverable 5] Return the left and right singular vectors associated to the largest singular value of the input matrix
def PowerMiniSVD( matrix, num_iterations = 10**3 ):
  u0, v0 = None, None
  ### BEGIN SOLUTION
  ### END SOLUTION
  return u0, v0



# [TODO: Deliverable 6] Given the SVD of a blur matrix, deblur the input image using a rank-reduced approximation of the blur matrix. 0 <= R <= 1 denotes the amount of energy to retain in your rank-reduced approximation.
def DeblurImageSVD( U, s, Vt, blurred_image, R):
  deblurred_img = None
  ### BEGIN SOLUTION
  ### END SOLUTION
  return deblurred_img



# Some example test routines for the deliverables. 
# Feel free to write and include your own tests here.
# Code in this main block will not count for credit, 
# but the collaboration and plagiarism policies still hold.
if __name__ == "__main__":
  
  # Load test data from file
  image, blurred_image, blurred_noisy_image = None, None, None
  with open('A3-test-data.npy', 'rb') as f:
    image = np.load(f)
    blurred_image = np.load(f)
    blurred_noisy_image = np.load(f)

  print("[Deliverables 1 and 2] Blur Matrix Construction with 2D Gaussian kernel")

  # Gaussian kernel 21 x 21, sigma = 1.0: this is the kernel we used to generate blurred_image
  sigma, k = 1.0, 21
  kernel2d = Gaussian2D(k, sigma)
  A = CreateBlurMatrix( kernel2d, image.shape[1], image.shape[0] )  
  # TODO: test your blurring results


  print("[Deliverables 3 and 4] Regularized Deblurring")
  # Generate and display a handful of deblurred images 
  
  cmap = plt.get_cmap('gray')
  _, plots = plt.subplots(2, 3, figsize=(10,7))
  plt.setp(plots, xticks=[], yticks=[])
  
  plots[0,0].set_title('original image - from file', size=8)
  plots[0,0].imshow(image, cmap, vmin=0, vmax=1)
  
  plots[0,1].set_title('blurred image - from file', size=8)
  plots[0,1].imshow(blurred_image, cmap, vmin=0, vmax=1)

  plots[0,2].set_title('blurred image + noise - from file', size=8)
  plots[0,2].imshow( blurred_noisy_image, cmap, vmin=0, vmax=1)

  plots[1,0].set_title('Deblurred (no regularization)', size=8)
  plots[1,0].imshow( DeblurImage(A, blurred_noisy_image), cmap, vmin=0, vmax=1)

  temp_img = DeblurImage(A, blurred_noisy_image, 0.05)
  plots[1,1].set_title("Deblurred (Tikonov with $\lambda = 0.05$; error = " + "{:.2f}".format(np.linalg.norm(temp_img - image)) + ")", size=8)
  plots[1,1].imshow( temp_img, cmap, vmin=0, vmax=1)

  temp_img = DeblurImageCompetition(A, blurred_noisy_image)
  plots[1,2].set_title("Deblurred (your best; error = " + "{:.2f}".format(np.linalg.norm(temp_img - image)) + ")", size=8)
  plots[1,2].imshow( temp_img, cmap, vmin=0, vmax=1)

  plt.show() # this is a blocking call; kill the plotting window to continue execution


  print("[Deliverables 5] Power Iteration for Mini-SVD")
  # Test for accuracy with a liberal floating-point threshold
  M = np.random.rand(20,20)
  M_u, M_s, M_Vt = np.linalg.svd(M)
  u0, v0 = PowerMiniSVD(M)
  print("Do left singular vectors match? " + str( np.all( np.abs( np.abs(M_u[:,0]) - np.abs(u0) ) < 1e-4 ) ) )
  print("Do right singular vectors match? " + str( np.all( np.abs( np.abs(M_Vt[0,:]) - np.abs(v0) ) < 1e-4 ) ) )


  print("[Deliverables 6] SVD Deblurring")
  U, s, Vt = np.linalg.svd(A) # This is expensive...
  
  cmap = plt.get_cmap('gray')
  _, plots = plt.subplots(1, 3, figsize=(10,4))
  plt.setp(plots, xticks=[], yticks=[])

  temp_img = DeblurImageSVD(U, s, Vt, blurred_noisy_image, 0.9)
  plots[0].set_title("Deblurred (90% SVD; error = " + "{:.2f}".format(np.linalg.norm(temp_img - image)) + ")", size=8)
  plots[0].imshow( temp_img, cmap, vmin=0, vmax=1)
  
  temp_img = DeblurImageSVD(U, s, Vt, blurred_noisy_image, 0.95)
  plots[1].set_title("Deblurred (95% SVD; error = " + "{:.2f}".format(np.linalg.norm(temp_img - image)) + ")", size=8)
  plots[1].imshow( temp_img, cmap, vmin=0, vmax=1)
  
  temp_img = DeblurImageSVD(U, s, Vt, blurred_noisy_image, 0.99)
  plots[2].set_title("Deblurred (99% SVD; error = " + "{:.2f}".format(np.linalg.norm(temp_img - image)) + ")", size=8)
  plots[2].imshow( temp_img, cmap, vmin=0, vmax=1)

  plt.show() # this is a blocking call; kill the plotting window to continue execution