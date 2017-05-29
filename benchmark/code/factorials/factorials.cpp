#include <algorithm>
#include <ctime>
#include <iostream>

long factorial(int number) {
	long n = 1;
	for (int i = 1; i < number; i++) {
		n = n * i;
	}
	return n;
}

int main() {
	long c = 0;
	for (int i = 1; i < 3000; ++i) {
		for (int j = 1; j < 1000; ++j) {
			c += factorial(j) + i;
		}
		c = c % 1000000;
	}
	std::cout << c << std::endl;
}
