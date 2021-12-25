#include <iostream>
#include <fstream>
#include <vector>

using grid_t = std::vector<std::vector<char>>;

bool next_step(grid_t& grid) {
	bool moved = false;
	grid_t new_grid = grid;
	for (std::size_t y = 0; y < grid.size(); ++y) {
		for (std::size_t x = 0; x < grid[y].size(); ++x) {
			if (grid[y][x] == '>') {
				std::size_t next_x = (x + 1) % grid[y].size();
				if (grid[y][next_x] == '.') {
					new_grid[y][next_x] = '>';
					new_grid[y][x] = '.';
					moved = true;
				}
			}
		}
	}
	grid = new_grid;
	for (std::size_t y = 0; y < grid.size(); ++y) {
		for (std::size_t x = 0; x < grid[y].size(); ++x) {
			if (grid[y][x] == 'v') {
				std::size_t next_y = (y + 1) % grid.size();
				if (grid[next_y][x] == '.') {
					new_grid[y][x] = '.';
					new_grid[next_y][x] = 'v';
					moved = true;
				}
			}
		}
	}
	grid = new_grid;
	return moved;
}

int main() {
	std::ifstream input("input/25.txt");
	std::string line;
	grid_t grid;
	while(std::getline(input, line)) {
		grid.emplace_back(line.begin(), line.end());
	}
	std::size_t step = 0;
	while(true) {
		++step;
		grid_t new_grid = grid;
		bool moved = next_step(new_grid);
		grid = new_grid;
		if (!moved) {
			break;
		}
	}
	std::cout << "Part 1: " << step << std::endl;
}