import socket
import struct
import cv2

# IP address and port number of the cameraDealerNode
IP = '192.168.88.201'
PORT = 2244

# Create a socket and bind it to the IP and port
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((IP, PORT))

while True:
    # Receive image data from the cameraDealerNode
    data, addr = s.recvfrom(1024)
    # Unpack the image size
    size = struct.unpack("<L", data[:4])[0]
    # Get the image data
    img_data = data[4:]
    # Decode the image data and convert it to a numpy array
    img_array = np.frombuffer(img_data, dtype=np.uint8)
    # Reshape the array to an image
    img = img_array.reshape(size[1], size[0], 3)
    # Display the image using OpenCV
    cv2.imshow("Image from cameraDealerNode", img)
    cv2.waitKey(1)

# Close the socket when the program is closed
s.close()
