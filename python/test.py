import numpy as alex
import time
import math
theta = 2.3562
theta2 = -2.3562
t1 = time.time()
for i in range(100000):
    angle = theta if theta>=0 else 6.283185-theta
    angle2 = theta2 if theta2>=0 else 6.283185+theta2
print(time.time()-t1)

t1 = time.time()
for i in range(100000):
    angle3 = (6.2831853+theta)%6.283185
    angle4 = (6.2831853+theta2)%6.283185
print(time.time()-t1)
print(angle, angle2, angle3, angle4)
t1 = time.time()
for i in range(10000):
    math.sin(i/100)
print(time.time()-t1)

t1 = time.time()
for i in range(1,10001):
    math.log(i/100)
print(time.time()-t1)

t1 = time.time()
for i in range(10000):
    i*i+3*i/100
print(time.time()-t1)

t1 = time.time()
for i in range(10000):
    math.exp(i/100)
print(time.time()-t1)