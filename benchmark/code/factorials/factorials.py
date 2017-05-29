#!/usr/bin/python

def factorial(n):
	f = 1
	for i in range(1, n):
		f *= i
	return f

c = 0
for i in range(0, 30):
	for j in range(1, 1000):
		c += factorial(j) + i
	c = c % 1000000

print(c)
