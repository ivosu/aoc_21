#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

bool is_opening(char c) {
	return c == '(' || c == '[' || c == '{' || c == '<';
}

char get_matching_closing(char c) {
	if (c == '(') {
		return ')';
	}
	if (c == '[') {
		return ']';
	}
	if (c == '{') {
		return '}';
	}
	if (c == '<') {
		return '>';
	}
	return ' ';
}

std::size_t get_currupted_score(char c) {
	if (c == ')') {
		return 3;
	}
	if (c == ']') {
		return 57;
	}
	if (c == '}') {
		return 1197;
	}
	if (c == '>') {
		return 25137;
	}
	return 0;
}

std::size_t get_incomplete_score(char c) {
	if (c == ')') {
		return 1;
	}
	if (c == ']') {
		return 2;
	}
	if (c == '}') {
		return 3;
	}
	if (c == '>') {
		return 4;
	}
	return 0;
}

int main() {
	std::ifstream input("input/10.txt");
	std::string line;
	std::size_t corrupted_score = 0;
	std::vector<std::size_t> incomplete_scores;
	while(std::getline(input, line)) {
		std::stack<char> s;
		bool corrupted = false;
		for (char c : line) {
			if (is_opening(c)) {
				s.push(c);
			} else if (get_matching_closing(s.top()) != c) {
				corrupted_score += get_currupted_score(c);
				corrupted = true;
				break;
			} else {
				s.pop();
			}
		}
		if (corrupted || s.empty()) {
			continue;
		}
		std::size_t incomplete_score = 0;
		while(!s.empty()) {
			incomplete_score *= 5;
			incomplete_score += get_incomplete_score(get_matching_closing(s.top()));
			s.pop();
		}
		incomplete_scores.push_back(incomplete_score);
	}
	std::cout << "Part 1: " << corrupted_score << std::endl;
	std::nth_element(incomplete_scores.begin(), std::next(incomplete_scores.begin(), incomplete_scores.size() / 2 - 1), incomplete_scores.end());
	std::cout << "Part 2: " << incomplete_scores.at(incomplete_scores.size() / 2) << std::endl;
}