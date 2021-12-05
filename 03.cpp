#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <map>

std::size_t count_ones(const std::vector<std::string>& numbers, std::size_t position) {
	return std::count_if(numbers.begin(), numbers.end(), [position](const std::string& bin_number) { return bin_number[position] == '1'; });
}

std::size_t count_zeroes(const std::vector<std::string>& numbers, std::size_t position) {
	return std::count_if(numbers.begin(), numbers.end(), [position](const std::string& bin_number) { return bin_number[position] == '0'; });
}

void erase_ones(std::vector<std::string>& numbers, std::size_t position) {
	numbers.erase(
		std::remove_if(
			numbers.begin(),
			numbers.end(),
			[position] (const std::string& bin_number) { return bin_number[position] == '1'; }),
		numbers.end()
	);
}

void erase_zeroes(std::vector<std::string>& numbers, std::size_t position) {
	numbers.erase(
		std::remove_if(
			numbers.begin(),
			numbers.end(),
			[position] (const std::string& bin_number) { return bin_number[position] == '0'; }),
		numbers.end()
	);
}

int convert_binary_string(const std::string& number) {
	int result = 0;
	for (char c : number) {
		result <<= 1;
		if (c == '1') {
			result += 1;
		}
	}
	return result;
}

constexpr std::size_t bit_count = 12;

int main() {
	std::ifstream input("input/03.txt");

	std::vector<std::string> possible_oxygen_rating;
	std::vector<std::string> possible_co2_rating;

	std::string binary_number;
	std::array<std::size_t, bit_count> ones_counter;
	ones_counter.fill(0);
	std::size_t total_binary_number_counter = 0;

	while (input >> binary_number) {
		possible_oxygen_rating.push_back(binary_number);
		possible_co2_rating.push_back(binary_number);
		++total_binary_number_counter;
		// process the number
		for (std::size_t i = 0; i < bit_count; ++i) {
			if (binary_number[i] == '1') {
				++ones_counter[i];
			}
		}
	}

	// Part 1
	int gamma = 0;
	int epsilon = 0;
	for (std::size_t i = 0; i < bit_count; ++i) {
		gamma <<= 1;
		epsilon <<= 1;
		if (ones_counter[i] * 2 > total_binary_number_counter) {
			gamma += 1;
		} else {
			epsilon += 1;
		}
	}
	std::cout << "Part 1:\n" << gamma * epsilon << std::endl;

	// Part 2
	for (std::size_t i = 0; i < bit_count && possible_oxygen_rating.size() > 1; ++i) {
		if (count_ones(possible_oxygen_rating, i) * 2 >= possible_oxygen_rating.size()) {
			erase_zeroes(possible_oxygen_rating, i);
		} else {
			erase_ones(possible_oxygen_rating, i);
		}
	}
	for (std::size_t i = 0; i < bit_count && possible_co2_rating.size() > 1; ++i) {
		if (count_zeroes(possible_co2_rating, i) * 2 <= possible_co2_rating.size()) {
			erase_ones(possible_co2_rating, i);
		} else {
			erase_zeroes(possible_co2_rating, i);
		}
	}
	std::cout << "Part 2:\n" << convert_binary_string(possible_oxygen_rating.front()) * convert_binary_string(possible_co2_rating.front()) << std::endl;
	return 0;
}
