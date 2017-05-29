#include <algorithm>
#include <ctime>
#include <iostream>

bool is_prime_fast(int number) {
	for (int k = 1; 36 * k * k - 12 * k < number; ++k) {
		if ((number % (6 * k + 1) == 0) or (number % (6 * k - 1) == 0)) {
			return false;
		}
	}
	return true;
}

int main() {
	int c = 2;
	for (int j = 5; j < 8000000; j += 6) {
		if (is_prime_fast(j)) c++;
		if (is_prime_fast(j + 2)) c++;
	}
	std::cout << c << std::endl;
}
