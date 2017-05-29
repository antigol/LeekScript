#!/usr/bin/python

def is_prime(number):
	k = 1
	while 36 * k * k - 12 * k < number:
		if number % (6 * k + 1) == 0 or number % (6 * k - 1) == 0:
			return False
		k = k + 1
	return True

c = 2
for i in range(5, 80000, 6):
	if is_prime(i):
		c = c + 1
	if is_prime(i + 2):
		c = c + 1
print(c)
