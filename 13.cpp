#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

struct coord {
	std::size_t x;
	std::size_t y;
};

int main() {
	std::ifstream input("input/13.txt");

	std::vector<coord> dots;
	std::vector<std::vector<bool>> paper;

	std::size_t x_max = 0;
	std::size_t y_max = 0;
	std::string line;
	while(true) {
		std::getline(input, line);
		if (line.empty()) {
			break;
		}
		std::stringstream ss(line);
		std::size_t x;
		std::size_t y;
		char c;
		ss >> x >> c >> y;
		dots.push_back({x,y});
		x_max = std::max(x, x_max);
		y_max = std::max(y, y_max);
	}
	paper.resize(x_max + 1, std::vector<bool>(y_max + 1, false));
	for (const auto&[x, y] : dots) {
		paper[x][y] = true;
	}

	bool first_fold = true;
	while(std::getline(input,line)) {
		// skip "fold along "
		char axis = line[11];
		// skip "fold along x="
		std::stringstream ss(line.substr(13));
		std::size_t fold;
		ss >> fold;
		if (axis == 'x') {
			// fold x
			for (std::size_t x = fold + 1; x <= x_max; ++x) {
				const std::vector<bool>& line = paper[x];
				std::vector<bool>& flipped_line = paper[fold - (x - fold)];
				for (std::size_t y = 0; y <= y_max; ++y) {
					if (line[y]) {
						flipped_line[y] = true;
					}
				}
			}
			x_max = fold;
		} else if (axis == 'y') {
			// fold y
			for (std::size_t x = 0; x <= x_max; ++x) {
				std::vector<bool>& line = paper[x];
				for (std::size_t y = fold + 1; y <= y_max; ++y) {
					if (line[y]) {
						line[fold - (y - fold)] = true;
					}
				}
			}
			y_max = fold;
		}
		if (first_fold) {
			std::size_t counter = 0;
			for (std::size_t x = 0; x <= x_max; ++x) {
				for (std::size_t y = 0; y <= y_max; ++y) {
					if (paper[x][y]) {
						++counter;
					}
				}
			}
			std::cout << "Part 1: " << counter << std::endl;
			first_fold = false;
		}
	}

	std::cout << "Part 2 :" << '\n';
	for (std::size_t y = 0; y <= y_max; ++y) {
		for (std::size_t x = 0; x <= x_max; ++x) {
			if (paper[x][y]) {
				std::cout << 'X';
			} else {
				std::cout << ' ';
			}
		}
		std::cout << '\n';
	}

}