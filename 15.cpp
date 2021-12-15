#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>
#include <chrono>

struct coord {
	std::size_t x;
	std::size_t y;

	bool operator==(const coord& o) const {
		return x == o.x && y == o.y;
	}
};

std::size_t find_lowest_risk_level(const std::vector<std::vector<std::size_t>>& map, const coord& starting_coord, const coord& target) {
	std::vector<std::vector<bool>> visited = {map.size(), std::vector<bool>(map.size(), false)};
	std::vector<std::pair<coord, std::size_t>> next_to_visit;
	// small optimization to eliminate more allocations, more of a guess than precise max number of queued elements
	next_to_visit.reserve(map.size() * 4);
	next_to_visit.emplace_back(starting_coord, 0);
	auto comp_func = [](const std::pair<coord, std::size_t>& a, const std::pair<coord, std::size_t>& b) { return a.second > b.second; };

	while(!next_to_visit.empty()) {
		// moves the "next" value to the back
		std::pop_heap(next_to_visit.begin(), next_to_visit.end(), comp_func);
		std::size_t x = next_to_visit.back().first.x;
		std::size_t y = next_to_visit.back().first.y;
		std::size_t current_length = next_to_visit.back().second;

		next_to_visit.pop_back();

		// check possible neighbours
		std::vector<coord> neighbours;
		neighbours.reserve(4);
		if (coord c{x - 1, y}; x > 0 && !visited[c.x][c.y]) {
			visited[c.x][c.y] = true;
			neighbours.push_back(c);
		}
		if (coord c = {x + 1, y}; x + 1 < map.size() && !visited[c.x][c.y]) {
			visited[c.x][c.y] = true;
			neighbours.push_back(c);
		}
		if (coord c = {x, y - 1}; y > 0 && !visited[c.x][c.y]) {
			visited[c.x][c.y] = true;
			neighbours.push_back(c);
		}
		if (coord c = {x, y + 1}; y + 1 < map.size() && !visited[c.x][c.y]) {
			visited[c.x][c.y] = true;
			neighbours.push_back(c);
		}
		// check neighbours
		for (const coord& c : neighbours) {
			std::size_t new_length = current_length + map[c.x][c.y];
			if (c == target) {
				return new_length;
			}
			next_to_visit.emplace_back(c, new_length);
			std::push_heap(next_to_visit.begin(), next_to_visit.end(), comp_func);
		}
	}
	return 0;
}

int main() {
	std::ifstream input("input/15.txt");
	std::vector<std::vector<std::size_t>> map;
	std::string line;
	while(std::getline(input, line)) {
		std::vector<std::size_t> line_v;
		line_v.reserve(line.size());
		for (char c : line) {
			line_v.push_back(c - '0');
		}
		map.push_back(line_v);
	}
	std::cout << "Part 1: " << find_lowest_risk_level(map, {0, 0}, {map.size() - 1, map.size() - 1}) << std::endl;

	std::vector<std::vector<std::size_t>> enlarged_map(map.size() * 5, std::vector<std::size_t>(map.size() * 5, 0));
	for (std::size_t x_dupl = 0; x_dupl < 5; ++x_dupl) {
		for (std::size_t y_dupl = 0; y_dupl < 5; ++y_dupl) {
			for (std::size_t x = 0; x < map.size(); ++x) {
				for (std::size_t y = 0; y < map.size(); ++y) {
					std::size_t new_val = map[x][y] + x_dupl + y_dupl;
					if	 (new_val > 9) {
						new_val -= 9;
					}
					enlarged_map[x + x_dupl * map.size()][y + y_dupl * map.size()] = new_val;
				}
			}
		}
	}
	std::cout << "Part 2: " << find_lowest_risk_level(enlarged_map, {0, 0}, {enlarged_map.size() - 1, enlarged_map.size() - 1}) << std::endl;
}