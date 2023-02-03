# [TODO] Rename this file to [your student ID].py

# DO NOT EDIT THESE IMPORT STATEMENTS!
import sys
import matplotlib.pyplot as plt   # plotting 
import numpy as np                # all of numpy...
del sys.modules["numpy"].fft      # ... except FFT helpers
##########################


# [TODO: Deliverable 1] A generalized discrete Fourier transform
# inSignal - 1D (sampled) input signal numpy array
# s - sign parameter with default value -1 for the DFT vs. iDFT setting
# returns the DFT of the input signal
def DFT(inSignal, s : int = -1): 
  y = np.zeros(inSignal.shape, dtype = complex)
  ### BEGIN SOLUTION
  ### END SOLUTION
  return y


# [TODO: Deliverable 2] The inverse DFT, relying on your generalized DFT routine above
# inSignal - complex-valued (sampled) 1D DFT input numpy array
# returns the iDFT of the input signal
def iDFT(inSignal : complex):
  ### BEGIN SOLUTION
  ### END SOLUTION


# [TODO: Deliverable 3] A generalized 2D discrete Fourier transform routine
# inSignal2D - 2D (sampled) input signal numpy array
# s - sign parameter with default value -1 for the DFT vs. iDFT setting
# returns the 2D DFT of the input signal
def DFT2D(inSignal2D, s : int = -1): 
  y = np.zeros(inSignal2D.shape, dtype = complex)
  ### BEGIN SOLUTION
  ### END SOLUTION
  return y


# [TODO: Deliverable 4] The inverse 2D DFT, relying on your 2D DFT routine
# inSignal2D - complex-valued (sampled) 2D DFT input array
# returns the 2D iDFT of the input signal
def iDFT2D(inSignal2D : complex):
  ### BEGIN SOLUTION
  ### END SOLUTION


# [TODO: Deliverable 5] Compute the Fourier-transformed convolution kernel
# blur_kernel2d - 2D n x n blur kernel in the primal domain
# image_shape - 2-tuple with the dimensions of the input image 
# (and output kernel and output image, and all of their Fourier transformed duals)
# DFT2D_func (optional) - the 2D DFT function that you'd like to use, 
# e.g., if you'd like to debug with a DFT implementation other than your own
def FourierKernelMatrix(blur_kernel2d, image_shape, DFT2D_func = DFT2D):
  K_hat = np.zeros( image_shape, dtype = complex )
  ### BEGIN SOLUTION
  ### END SOLUTION
  return K_hat


# [TODO: Deliverable 6] Compute the Laplace-regularized Fourier-transformed deconvolution kernel
# blur_kernel2d - 2D n x n blur kernel in the primal domain
# image_shape - 2-tuple with the dimensions of the input image 
# (and output kernel and output image, and all of their Fourier transformed duals)
# DFT2D_func (optional) - the 2D DFT function that you'd like to use, 
# e.g., if you'd like to debug with a DFT implementation other than your own
def LaplacianInverseFourierKernel(blur_kernel2d, image_shape, DFT2D_func = DFT2D):
  K_hat_reg_inv = np.zeros( image_shape, dtype = complex)
  ### BEGIN SOLUTION
  ### END SOLUTION
  return K_hat_reg_inv



# Some example test routines for the deliverables. 
# It's almost impossible to complete the assignment without
# including your own additional tests, here.
# Code in this main block will not count for credit, 
# but the collaboration and plagiarism policies still hold.
if __name__ == "__main__":
  
  # Load test data from file
  blurred_noisy_image, kernel2d = None, None
  with open('A4-test-data.npy', 'rb') as f:
    blurred_noisy_image = np.load(f)
    kernel2d = np.load(f)

  print("[Deliverables 1 and 2] 1D Discrete Fourier Transform")
  test_signal_1d = np.random.rand(32)
  print( np.allclose( test_signal_1d,
                      iDFT( DFT(test_signal_1d) ) ) );
  ### TODO: consider writing more extensive tests, here...



  print("[Deliverables 3 and 4] 2D Discrete Fourier Transform")
  test_signal_2d = np.random.rand(32, 32)
  print( np.allclose( test_signal_2d,
                      iDFT2D( DFT2D(test_signal_2d) ) ) );
  ### TODO: consider writing more extensive tests, here...

  

  print("[Deliverable 5] Fourier Convolution Kernel Matrix")
  # FYI, our test data blur kernel (kernel2d) is a 61 x 61 2D Isotropic 
  # Gaussian with sigma = 10.0, i.e., Gaussian2D(61, 10.) from A3
  test_input_image = np.zeros((128,128))
  test_input_image[50:80,50:80] = 1
  K_hat = FourierKernelMatrix(kernel2d, test_input_image.shape, DFT2D)
  blurred_image = np.real( iDFT2D( DFT2D(test_input_image) * K_hat) )

  cmap = plt.get_cmap('gray')
  _, plots = plt.subplots(2, 1, figsize=(10,7))
  plt.setp(plots, xticks=[], yticks=[])
  plots[0].set_title('test input image', size=8)
  plots[0].imshow(test_input_image, cmap, vmin=0, vmax=1)
  plots[1].set_title('blurred output image', size=8)
  plots[1].imshow(blurred_image, cmap, vmin=0, vmax=1)
  plt.show() # this is a blocking call; kill the plotting window to continue execution
  ### TODO: consider writing more extensive tests, here...


  print("[Deliverable 6] Laplacian-regularized Fourier Deconvolution")
  K_hat_reg_inv = LaplacianInverseFourierKernel(kernel2d, blurred_noisy_image.shape, DFT2D)
  ### TODO: consider writing more extensive tests, here...

  cmap = plt.get_cmap('gray')
  _, plots = plt.subplots(1, 2, figsize=(10,7))
  plt.setp(plots, xticks=[], yticks=[])
  
  plots[0].set_title('blurred image + noise - from file', size=8)
  plots[0].imshow( blurred_noisy_image, cmap, vmin=0, vmax=1)
  
  deblurred_output = np.real( iDFT2D( DFT2D( blurred_noisy_image ) * K_hat_reg_inv ) )
  plots[1].set_title('FFT Deblurred (Laplacian regularization)', size=8)
  plots[1].imshow( deblurred_output, cmap, vmin=0, vmax=1)
  
  plt.show() # this is a blocking call; kill the plotting window to continue execution

  quit()