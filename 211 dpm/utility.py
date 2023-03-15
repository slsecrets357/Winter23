
#define colors in rgb space
BLUE = [0, 0, 255]
RED = [255, 0, 0]
GREEN = [0, 255, 0]
YELLOW = [255, 255, 0]
ORANGE = [255, 165, 0]
MAGENTA = [255, 0, 255]
default_colors = [BLUE, RED, GREEN, YELLOW, ORANGE, MAGENTA]
color_names = ['blue', 'red', 'green', 'yellow', 'orange', 'magenta']

def euclidean_distance(color_a, color_b):
    """
    Calculate the euclidean distance between two colors.
    """
    r_diff = (color_a[0] - color_b[0]) ** 2
    g_diff = (color_a[1] - color_b[1]) ** 2
    b_diff = (color_a[2] - color_b[2]) ** 2
    return (r_diff + g_diff + b_diff) ** 0.5

def sort_color(rgbValue):
    #classify color based on euclidean distance to default colors
    distances = []
    for color in default_colors:
        dist = euclidean_distance(color, rgbValue)
        distances.append(dist)
    index = distances.index(min(distances))
    # distances = np.linalg.norm(default_colors - rgbValue, axis=1)
    # index = np.argmin(distances)
    print("Color detected: " + color_names[index])
    return index, color_names[index]

def lane_follower(colorIndex):
    #takes as input an rgb value and returns a steering angle
    #used to follow a lane where left is red and right is blue
    #if color is green, stop
    #classify color
    
    #if color is red, turn left
    if colorIndex == 1:
        return turn_left()
    elif colorIndex == 0:
        return turn_right()
    elif colorIndex == 2:
        return stop()
    else:
        return go_straight()

# functions returning steering rate of left and right wheels
# to be called in state machine (not yet implemented)
# steering rate between 0 and 1, needs to be scaled to actual speed
def stop():
    #stop the car
    return 0,0

def go_straight():
    #go straight
    return 1,1

def turn_right():
    #turn right
    return 1,0.5

def turn_left():
    #turn left
    return 0.5,1
    
    