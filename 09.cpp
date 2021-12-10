#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <set>
#include <numeric>

struct coord {
	std::size_t x;
	std::size_t y;

	bool operator<(const coord& o) const {
		return x < o.x || (x == o.x && y < o.y);
	}
};

int main() {
	std::ifstream input("input/09.txt");
	std::vector<std::string> height_map;
	std::string line;
	while(input >> line) {
		height_map.push_back(line);
	}
	std::size_t sum = 0;
	std::vector<coord> low_points;
	std::vector<std::set<coord>> basins;
	for (std::size_t i = 0; i < height_map.size(); ++i) {
		for (std::size_t j = 0; j < height_map[i].size(); ++j) {
			char c = height_map[i][j];
			if (i > 0 && height_map[i-1][j] <= c) {
				continue;
			}
			if (i + 1 < height_map.size() && height_map[i+1][j] <= c) {
				continue;
			}
			if (j > 0 && height_map[i][j - 1] <= c) {
				continue;
			}
			if (j + 1 < height_map[i].size() && height_map[i][j + 1] <= c) {
				continue;
			}
			low_points.push_back(coord{i, j});
			sum += c - '0' + 1;
		}
	}
	for (const auto& l  : low_points) {
		std::set<coord> visited = {l};
		std::queue<coord> visit_next;
		visit_next.push(l);
		while(!visit_next.empty()) {
			std::size_t x = visit_next.front().x;
			std::size_t y = visit_next.front().y;
			visit_next.pop();
			if (coord c{x - 1, y}; x > 0 && height_map[c.x][c.y] != '9' && visited.insert(c).second) {
				visit_next.push(c);
			}
			if (coord c = {x + 1, y}; x + 1 < height_map.size() && height_map[c.x][c.y] != '9' && visited.insert(c).second) {
				visit_next.push(c);
			}
			if (coord c = {x, y-1}; y > 0 && height_map[c.x][c.y] != '9' && visited.insert(c).second) {
				visit_next.push(c);
			}
			if (coord c = {x, y+1}; y + 1 < height_map[c.x].size() && height_map[c.x][c.y] != '9' && visited.insert(c).second) {
				visit_next.push(c);
			}
		}
		basins.push_back(visited);
	}
	std::nth_element(
		basins.begin(),
		std::next(basins.begin(), 2),
		basins.end(),
		[](const std::set<coord>& a, const std::set<coord>& b) { return a.size() > b.size(); }
	);

	std::cout << "Part 1: " << sum << std::endl;
	std::cout
		<< "Part 2: "
		<< std::accumulate(
			basins.begin(),
			std::next(basins.begin(), 3),
			1,
			[](std::size_t i, const std::set<coord>& o) { return i * o.size(); }
		)
		<< std::endl;
}