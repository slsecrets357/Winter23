import numpy as np

#define colors in rgb space
BLUE = [0, 0, 255]
RED = [255, 0, 0]
GREEN = [0, 255, 0]
YELLOW = [255, 255, 0]
ORANGE = [255, 165, 0]
MAGENTA = [255, 0, 255]
default_colors = np.array([BLUE, RED, GREEN, YELLOW, ORANGE, MAGENTA])
color_names = ['blue', 'red', 'green', 'yellow', 'orange', 'magenta']

def sort_color(rgbValue):
    #classify color based on euclidean distance to default colors
    distances = np.linalg.norm(default_colors - rgbValue, axis=1)
    index = np.argmin(distances)
    return index, color_names[index]
    
    