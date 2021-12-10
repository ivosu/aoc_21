#include <iostream>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>

constexpr std::size_t input_part_size = 10;
constexpr std::size_t output_part_size = 4;

using signals_t = std::set<char>;
using input_signals_t = std::array<signals_t, input_part_size>;
using output_signals_t = std::array<signals_t, output_part_size>;

struct signals {
	input_signals_t input_signals;
	output_signals_t output_signals;
};

bool is_subset(const signals_t& set, const signals_t& subset) {
	return std::all_of(subset.begin(), subset.end(), [set](char c){ return set.find(c) != set.end(); });
}

int main() {
	std::ifstream input("input/08.txt");
	std::size_t part_1 = 0;
	std::size_t part_2 = 0;

	std::string line;
	while(std::getline(input, line)) {
		std::stringstream line_stream(line);
		signals current_signals;
		std::map<signals_t, short> decoded_signals;
		std::set<char> four_signal;
		std::set<char> seven_signal;
		std::set<char> four_minus_seven_signal;
		for (std::size_t i = 0; i < input_part_size; ++i) {
			std::string signals;
			line_stream >> signals;
			signals_t& current_signal = current_signals.input_signals.at(i);
			current_signal.insert(signals.begin(), signals.end());
			if (current_signal.size() == 2) {
				decoded_signals[current_signal] = 1;
			} else if (current_signal.size() == 3) {
				seven_signal.insert(current_signal.begin(), current_signal.end());
				decoded_signals[current_signal] = 7;
			} else if (current_signal.size() == 4) {
				four_signal.insert(current_signal.begin(), current_signal.end());
				decoded_signals[current_signal] = 4;
			} else if (current_signal.size() == 7) {
				decoded_signals[current_signal] = 8;
			}
		}
		// find the difference between 4 and 7
		for (char c : four_signal) {
			if (seven_signal.find(c) == seven_signal.end()) {
				four_minus_seven_signal.insert(c);
			}
		}

		// process all inputs
		for (const signals_t& signals : current_signals.input_signals) {
			if (decoded_signals.find(signals) != decoded_signals.end()) {
				// one of the simply decoded ones
				continue;
			}
			if (signals.size() == 5) {
				// 3, 2 or 5
				if (is_subset(signals, seven_signal)) {
					decoded_signals[signals] = 3;
				} else if (is_subset(signals, four_minus_seven_signal)) {
					decoded_signals[signals] = 5;
				} else {
					decoded_signals[signals] = 2;
				}
			} else if (signals.size() == 6) {
				// 0, 6 or 9
				if (is_subset(signals, four_signal)) {
					decoded_signals[signals] = 9;
				} else if (is_subset(signals, seven_signal)) {
					decoded_signals[signals] = 0;
				} else {
					decoded_signals[signals] = 6;
				}
			}
		}
		line_stream.get(); // skip ' '
		line_stream.get(); // skip '|'
		line_stream.get(); // skip ' '
		std::size_t output_number = 0;
		for (std::size_t i = 0; i < output_part_size; ++i) {
			std::string signals;
			line_stream >> signals;
			signals_t& current_signal = current_signals.output_signals.at(i);
			current_signal.insert(signals.begin(), signals.end());
			output_number *= 10;
			output_number += decoded_signals[current_signal];
			if(current_signal.size() == 2 || current_signal.size() == 3 || current_signal.size() == 4 || current_signal.size() == 7) {
				++part_1;
			}
		}
		std::cout << output_number << std::endl;
		part_2 += output_number;
	}
	std::cout << "Part 1: " << part_1 << std::endl;
	std::cout << "Part 2: " << part_2 << std::endl;
}