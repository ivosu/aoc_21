#include <iostream>
#include <fstream>
#include <vector>
#include <map>

std::map<std::pair<char, char>, std::size_t> next_step(const std::map<std::pair<char, char>, std::size_t>& pairs, const std::map<std::pair<char, char>, char>& rules) {
	std::map<std::pair<char, char>, std::size_t> new_pairs;
	for (const auto& [pair, count] : pairs) {
		std::pair<char, char> pair_1 = {pair.first, rules.at(pair)};
		std::pair<char, char> pair_2 = {rules.at(pair), pair.second};
		if (new_pairs.find(pair_1) == new_pairs.end()) {
			new_pairs[pair_1] = count;
		} else {
			new_pairs[pair_1] += count;
		}

		if (new_pairs.find(pair_2) == new_pairs.end()) {
			new_pairs[pair_2] = count;
		} else {
			new_pairs[pair_2] += count;
		}
	}
	return new_pairs;
}

std::size_t calculate_answer(const std::map<std::pair<char, char>, std::size_t>& pairs, char first, char last) {
	std::map<char, std::size_t> counts;
	for (const auto& [pair, count] : pairs) {
		if (counts.find(pair.first) == counts.end()) {
			counts[pair.first] = count;
		} else {
			counts[pair.first] += count;
		}
		if (counts.find(pair.second) == counts.end()) {
			counts[pair.second] = count;
		} else {
			counts[pair.second] += count;
		}
	}
	std::size_t max = 0;
	std::size_t min = std::numeric_limits<std::size_t>::max();
	for (const auto&[c, count]: counts) {
		max = std::max(max, count/2 + (c == first ? 1 : 0) + (c == last ? 1 : 0));
		min = std::min(min, count/2 + (c == first ? 1 : 0) + (c == last ? 1 : 0));
	}
	return max - min;
}

int main() {
	std::ifstream input("input/14.txt");
	std::string polymer;
	std::getline(input, polymer);
	input.get(); // skip empty line
	std::map<std::pair<char, char>, char> rules;
	std::string line;
	while(std::getline(input, line)) {
		rules[{line[0], line[1]}] = line[6];
	}

	std::map<std::pair<char, char>, std::size_t> pair_counts;
	for (std::size_t i = 0; i + 1 < polymer.size(); ++i) {
		std::pair<char, char> pair = {polymer[i], polymer[i+1]};
		if (pair_counts.find(pair) == pair_counts.end()) {
			pair_counts[pair] = 1;
		} else {
			++pair_counts[pair];
		}
	}

	for (std::size_t step = 0; step < 10; ++step) {
		pair_counts = next_step(pair_counts, rules);
	}
	std::cout << "Part 1: " << calculate_answer(pair_counts, polymer.front(), polymer.back()) << std::endl;
	// add 30 more steps (40 total) for part 2
	for (std::size_t step = 0; step < 30; ++step) {
		pair_counts = next_step(pair_counts, rules);
	}
		std::cout << "Part 2: " << calculate_answer(pair_counts, polymer.front(), polymer.back()) << std::endl;
}