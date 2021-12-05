#include <iostream>
#include <fstream>
#include <regex>
#include <set>

struct coords {
	int x;
	int y;
	bool operator<(const coords& o) const {
		return std::tie(x, y) < std::tie(o.x, o.y);
	}
};

struct line {
	coords from;
	coords to;
};

void add_line_tile(coords coord, std::map<coords, int>& map) {
	if (auto it = map.find(coord); it == map.end()) {
		map[coord] = 1;
	} else {
		++(it->second);
	}
}

int get_direction(int from, int to) {
	if (from == to) {
		return 0;
	} else if (from < to) {
		return 1;
	}
	return -1;
}

int main() {
	std::ifstream input("input/05.txt");
	std::string line_str;
	std::vector<line> lines;
	while(std::getline(input, line_str)) {
		std::stringstream ss(
			std::regex_replace( // remove the arrow
				std::regex_replace(line_str, std::regex(","), " "), // remove the comma
				std::regex(" -> "),
				" "
			)
		);
		line current_line;
		ss >> current_line.from.x >> current_line.from.y >> current_line.to.x >> current_line.to.y;
		lines.push_back(current_line);
	}
	std::map<coords, int> crossings;
	for (const line& l : lines) {
		// uncomment for part 1
//		if (l.from.x != l.to.x && l.from.y != l.to.y) {
//			continue;
//		}
		int dx = get_direction(l.from.x, l.to.x);
		int dy = get_direction(l.from.y, l.to.y);
		for (int x = l.from.x, y = l.from.y; x != l.to.x || y != l.to.y; x += dx, y += dy) {
			add_line_tile({x, y}, crossings);
		}
		// the for loop doesn't add the last tile
		add_line_tile({l.to.x, l.to.y}, crossings);
	}
	int counter = 0;
	for (const auto& i : crossings) {
		if (i.second >= 2) {
			++counter;
		}
	}
	std::cout << counter << std::endl;
}