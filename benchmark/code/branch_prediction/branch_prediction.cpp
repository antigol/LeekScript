#include <algorithm>
#include <ctime>
#include <iostream>

int main() {
    const unsigned arraySize = 32768;
    int data[arraySize];
    for (unsigned c = 0; c < arraySize; ++c) {
        data[c] = std::rand() % 256;
	}
    std::sort(data, data + arraySize);

    long sum = 0;
    for (unsigned i = 0; i < 70000; i++) {
        for (unsigned c = 0; c < arraySize; ++c) {
            if (data[c] >= 128)
                sum += data[c];
        }
    }
    std::cout << sum << std::endl;
}
