#!/usr/bin/python
import random

size = 32768
data = []
for i in range(0, size):
	data.append(random.randint(0, 255))
data.sort()

sum = 0

for i in range(0, 700):
	for c in range(0, size):
		if data[c] >= 128:
			sum += data[c]

print(sum)
