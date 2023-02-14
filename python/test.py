import numpy as alex
import time
import math
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