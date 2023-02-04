#!/usr/bin/env python3


import cv2
import numpy as np
from pynput import keyboard

class CameraHandler():
    # ===================================== INIT==========================================
    def __init__(self):
        self.allKeys   = ['i','j','k','l','1','2','3','4','5','6','7','8','p','t','f','g','h','e','r']
        self.point = np.array([0.5,0.5])
        self.res = 3
        self.threshold = 100
        self.minlength = 30
        self.t = 3
        self.imgL = np.zeros((640, 480))
        self.color_e1 = (255,0,0)
        self.color_e2 = (255,0,0)
        self.error_p = np.array([0.1,0.9])
        self.error_w = 0.2
        self.lane_center = 0
        self.left=[]
        self.right=[]
        self.cv_image = np.zeros((640, 480))
        self.image_sub = cv2.imread('test2.jpg')        

        #values
        #     [1671916647.445153, 2589.654000]: testing
        # point,res,threshold,minlength [0.5 0.3] 3 100 10 [0.05 0.77]
    def detect_lines(self):
        img = self.image_sub
        img_edge = cv2.Canny(cv2.GaussianBlur(cv2.cvtColor(img, cv2.COLOR_BGR2GRAY),(5,5),0),50,150)
        mask = np.zeros_like(img_edge)
        h = img_edge.shape[0]
        w = img_edge.shape[1]
        poly = np.array([[(int(0*w),int(0.9*h)),(int(1*w),int(0.9*h)),(int(self.point[0]*w),int(self.point[1]*h))]])
        cv2.fillPoly(mask,poly,255)
        img_roi = cv2.bitwise_and(img_edge,mask)
        lines = cv2.HoughLinesP(img_roi,self.res,np.pi/180,self.threshold,minLineLength=self.minlength)
        img_lines = np.copy(img)
        print("here:",(int(0*w),int(0.9*h)),(int(self.point[0]*w),int(self.point[1]*h)))
        cv2.line(img_lines,(int(0*w),int(0.9*h)),(int(self.point[0]*w),int(self.point[1]*h)),(255,255,255),1)
        cv2.line(img_lines,(int(1*w),int(0.9*h)),(int(self.point[0]*w),int(self.point[1]*h)),(255,255,255),1)
        cv2.circle(img_lines, (int(self.point[0]*w),int(self.point[1]*h)), radius=3, color=(0,0,0), thickness=-1)
        self.color_e1 = (255,0,0)
        self.color_e2 = (255,0,0)
        # self.lines = len(lines)
        if lines is not None:
            for line in lines:
                x1,y1,x2,y2 = line.reshape(4)
                if abs(y2 - y1) > 0.01:
                    m = (x2-x1)/(y2-y1)
                    p_y = int(self.error_p[1]*h)
                    p_x = int(x1 + (p_y-y1)*m)
                    if x1 < w/2:
                        color = (0,0,255)
                        if p_x < int((self.error_p[0]+self.error_w)*w) and p_x > int(self.error_p[0]*w):
                            self.color_e1 = (0,255,0)
                            self.left.append(p_x)
                    else:
                        color = (255,255,0)
                        if p_x < int((1-self.error_p[0])*w) and p_x > int((1-self.error_p[0]-self.error_w)*w):
                            self.color_e2 = (0,255,0)
                            self.right.append(p_x)
                    cv2.circle(img_lines, (p_x,p_y), radius=2, color=(0,0,0), thickness=-1)
                    cv2.line(img_lines,(x1,y1),(x2,y2),color,self.t)
        cv2.line(img_lines,(int(self.error_p[0]*w),int(self.error_p[1]*h)),(int((self.error_p[0]+self.error_w)*w),int(self.error_p[1]*h)),self.color_e1,1)
        cv2.line(img_lines,(int(self.error_p[0]*w),int((self.error_p[1]-0.05)*h)),(int(self.error_p[0]*w),int((self.error_p[1]+0.05)*h)),self.color_e1,1)
        cv2.line(img_lines,(int((self.error_p[0]+self.error_w)*w),int((self.error_p[1]-0.05)*h)),(int((self.error_p[0]+self.error_w)*w),int((self.error_p[1]+0.05)*h)),self.color_e1,1)
        cv2.line(img_lines,(int((1-self.error_p[0]-self.error_w)*w),int(self.error_p[1]*h)),(int((1-self.error_p[0])*w),int(self.error_p[1]*h)),self.color_e2,1)
        cv2.line(img_lines,(int((1-self.error_p[0]-self.error_w)*w),int((self.error_p[1]-0.05)*h)),(int((1-self.error_p[0]-self.error_w)*w),int((self.error_p[1]+0.05)*h)),self.color_e2,1)
        cv2.line(img_lines,(int((1-self.error_p[0])*w),int((self.error_p[1]-0.05)*h)),(int((1-self.error_p[0])*w),int((self.error_p[1]+0.05)*h)),self.color_e2,1)
        self.left_lane = np.mean(self.left)
        self.right_lane = np.mean(self.right)
        if self.left_lane is None:
            self.left_lane = 0
        if self.right_lane is None:
            self.right_lane = 640
        self.lane_center = (self.left_lane+self.right_lane)/2
        self.imgL = img_lines

    def callback(self, data):
        """
        :param data: sensor_msg array containing the image in the Gazsbo format
        :return: nothing but sets [cv_image] to the usefull image that can be use in opencv (numpy array)
        """
        self.detect_lines()
        cv2.imshow("Frame preview", self.imgL)
        key = cv2.waitKey(1)
    # ===================================== KEY PRESS ====================================
    def keyPress(self,key):
        """Processing the key pressing 
        
        Parameters
        ----------
        key : pynput.keyboard.Key
            The key pressed
        """                                     
        try: 
            print("=========== REMOTE CONTROL ============"+'\n'+
            
                "point[0]           "+str(self.point[0])+  '     [L/J]' +
                "\npoint[1]         "+str(self.point[1])+  '     [I/K]' +
                # "\nres              "+str(self.res)+       '     [1/2]' +
                # "\nthreshold        "+str(self.threshold)+ '     [3/4]' +
                # "\nminlength        "+str(self.minlength)+ '     [5/6]' +
                # "\nt                "+str(self.t)+         '     [6/7]' +
                "\nerror_p[0]       "+str(self.error_p[0])+'     [h/f]' +
                "\nerror_p[1]       "+str(self.error_p[1])+'     [g/t]' +
                "\nerror_w          "+str(self.error_w)+   '     [e/r]' + 
                "\nlane_center      "+str(self.lane_center)+        
                "\nlane_left        "+str(self.left_lane)+        
                "\nlane_right       "+str(self.right_lane)        
            )                                           
            if key.char in self.allKeys:
                if key.char == 'k' and self.point[0] < 1:
                    self.point[1] += 0.01
                elif key.char == 'i' and self.point[0] > 0:
                    self.point[1] -= 0.01
                elif key.char == 'l' and self.point[1] < 1:
                    self.point[0] += 0.01
                elif key.char == 'j' and self.point[1] > 0:
                    self.point[0] -= 0.01
                elif key.char == '1':
                    self.res += 1
                elif key.char == '2' and self.res > 1:
                    self.res -= 1
                elif key.char == '3':
                    self.threshold += 5
                elif key.char == '4' and self.threshold > 5:
                    self.threshold -= 5
                elif key.char == '5':
                    self.minlength += 1
                elif key.char == '6' and self.minlength > 5:
                    self.minlength -= 1
                elif key.char == '7':
                    self.t += 1
                elif key.char == '8' and self.t > 1:
                    self.t -= 1
                elif key.char == 'g' and self.error_p[1] < 1:
                    self.error_p[1] += 0.01
                elif key.char == 't' and self.error_p[0] > 0:
                    self.error_p[1] -= 0.01
                elif key.char == 'h' and self.error_p[1] < 1:
                    self.error_p[0] += 0.01
                elif key.char == 'f' and self.error_p[1] > 0:
                    self.error_p[0] -= 0.01
                elif key.char == 'e' and self.error_w < 1-self.error_p[0]:
                    self.error_w += 0.01
                elif key.char == 'r' and self.error_w > 0.01:
                    self.error_w -= 0.01
                elif key.char == 'p':
                    nod.detect_lines()
                    cv2.imshow("Frame preview", nod.imgL)
                    key = cv2.waitKey(10)
        except: pass
    
            
if __name__ == '__main__':
    nod = CameraHandler()
    print("hi")
    nod.detect_lines()
    with keyboard.Listener(on_press = nod.keyPress) as listener: 
        listener.join()
