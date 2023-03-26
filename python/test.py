import numpy as alex
import time
import math
table1 = alex.zeros(1500)
table2 = alex.zeros(1500)
for i in range(1500):
    table1[i] = i/1000
    table2[i] = math.exp(3.57*0.001*i-4.3)
t1 = time.time()
for j in range(1000):
    for i in table1:
        i2 = table2[int(i*1000)]
print(time.time()-t1)

t2 = time.time()
for j in range(1000):
    for i in table1:
        i2 = math.exp(3.57*i-4.3)
print(time.time()-t2)
    
# a = alex.random.randint(low=0, high=256, size=(640,480,3))
# shape = a.shape
# t1 = time.time()
# for i in range(10000):
#     a.flatten()
#     a = a.reshape(shape).astype(alex.uint8)
# print("time: ", time.time() -t1)
    
# theta = 2.3562
# theta2 = -2.3562
# t1 = time.time()
# for i in range(100000):
#     angle = theta if theta>=0 else 6.283185-theta
#     angle2 = theta2 if theta2>=0 else 6.283185+theta2
# print(time.time()-t1)
# 
# t1 = time.time()
# for i in range(100000):
#     angle3 = (6.2831853+theta)%6.283185
#     angle4 = (6.2831853+theta2)%6.283185
# print(time.time()-t1)
# print(angle, angle2, angle3, angle4)
# t1 = time.time()
# for i in range(10000):
#     math.sin(i/100)
# print(time.time()-t1)
# 
# t1 = time.time()
# for i in range(1,10001):
#     math.log(i/100)
# print(time.time()-t1)
# 
# t1 = time.time()
# for i in range(10000):
#     i*i+3*i/100
# print(time.time()-t1)
# 
# t1 = time.time()
# for i in range(10000):
#     math.exp(i/100)
# print(time.time()-t1)