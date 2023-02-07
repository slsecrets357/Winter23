import numpy as alex
import time
import math
t1 = time.time()
for i in range(10000):
    math.sin(i)
print(time.time()-t1)