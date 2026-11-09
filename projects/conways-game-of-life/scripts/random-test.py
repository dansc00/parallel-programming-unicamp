import random
import sys
x_len = int(sys.argv[1])  
y_len = int(sys.argv[2])  
ocuppacion = int(sys.argv[3])  
random.seed(0)
print(1)
print(y_len)
print(x_len)
for y in range(y_len):
	for x in range(x_len):
		if random.random() < ocuppacion / 100:
			print(f"({x}, {y})")