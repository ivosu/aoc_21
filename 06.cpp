#include <iostream>
#include <fstream>
#include <array>
#include <numeric>

int main() {
	std::ifstream input("input/06.txt");
	std::array<std::size_t, 9> lantern_fish = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	std::size_t number;
	std::size_t counter = 0;
	while(input >> number) {
		++lantern_fish[number];
		// count the original
		++counter;
		input.get();
	}
	for (int generation = 0; generation < 256; ++generation) {
		std::size_t zeros = lantern_fish[0];
		for (std::size_t i = 0; i < 8; ++i) {
			lantern_fish[i] = lantern_fish[i + 1];
		}
		lantern_fish[6] += zeros;
		// spawn in new ones
		lantern_fish[8] = zeros;
		// count these new ones
		counter += zeros;
	}
	std::cout << counter << std::endl;
}
