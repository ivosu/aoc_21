#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <optional>
#include <cmath>

bool is_inside(int x, int y, int x_min, int x_max, int y_min, int y_max) {
	return y >= y_min && x >= x_min && y <= y_max && x <= x_max;
}

std::optional<int> launch(int dx, int dy, int x_min, int x_max, int y_min, int y_max) {
	int x = 0;
	int y = 0;
	int max_y = 0;
	while (true) {
		if (dx > 0) {
			x += dx--;
		}
		y += dy--;
		max_y = std::max(max_y, y);
		if (is_inside(x, y, x_min, x_max, y_min, y_max)) {
			return max_y;
		}
		if (dy < 0 && y < y_min || x > x_max) {
			return std::nullopt;
		}
	}
}

int main() {
	std::ifstream input("input/17.txt");
	std::string line;
	std::getline(input, line);
	std::stringstream ss(line.substr(15));
	int x_min;
	int x_max;
	int y_min;
	int y_max;
	ss >> x_min;
	ss.get();
	ss.get();
	ss >> x_max;
	ss.get();
	ss.get();
	ss.get();
	ss.get();
	ss >> y_min;
	ss.get();
	ss.get();
	ss >> y_max;
	std::size_t counter = 0;
	int max_y = std::numeric_limits<int>::min();
	for (int x = static_cast<int>(std::sqrt(2 * x_min)); x <= x_max; ++x) {
		for (int y = y_min; y < -y_min; ++y) {
			if (auto i = launch(x, y, x_min, x_max, y_min, y_max)) {
				++counter;
				max_y = std::max(max_y, *i);
			}
		}
	}
	std::cout << "Part 1: " << max_y << std::endl;
	std::cout << "Part 2: " << counter << std::endl;
}