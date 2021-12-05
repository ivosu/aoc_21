#include <iostream>
#include <fstream>
#include <vector>

int main() {
	std::ifstream input("input/01.txt");
	std::vector<int> numbers;
	int number;
	while(input >> number) {
		numbers.push_back(number);
	}
	auto it = std::next(numbers.begin(), 3); // for part 1 remove the 3 argument
	auto prev = numbers.begin();
	int counter = 0;
	for (; it != numbers.end(); ++it, ++prev) {
		if (*it > *prev) {
			counter++;
		}
	}
	std::cout << counter << std::endl;
}
