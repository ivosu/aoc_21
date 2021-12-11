#include <iostream>
#include <fstream>
#include <array>
#include <set>
#include <queue>

constexpr std::size_t grid_size = 10;

using grid_t = std::array<std::array<int, grid_size>, grid_size>;

struct coord {
	std::size_t x;
	std::size_t y;

	bool operator<(const coord& o) const {
		return x < o.x || (x == o.x && y < o.y);
	}
};

// returns number of flashes in this step
std::size_t do_one_step(grid_t& grid) {
	std::set<coord> flashing;
	std::queue<coord> to_flash;
	// increase level off all
	for (std::size_t y = 0; y < grid_size; ++y) {
		for (std::size_t x = 0; x < grid_size; ++x) {
			if (++grid[x][y] > 9) {
				// mark ready to flash
				flashing.insert({x, y});
				to_flash.push({x, y});
			}
		}
	}
	while(!to_flash.empty()) {
		std::size_t x = to_flash.front().x;
		std::size_t y = to_flash.front().y;
		to_flash.pop();
		// check neighbours
		if (coord c{x - 1, y}; x > 0 && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x + 1, y}; x + 1 < grid_size && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x, y - 1}; y > 0 && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x, y + 1}; y + 1 < grid_size && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		// check diagonal neighbours
		if (coord c{x - 1, y - 1}; x > 0 && y > 0 && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x + 1, y - 1}; x + 1 < grid_size && y > 0 && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x - 1, y + 1}; x > 0 && y + 1 < grid_size && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
		if (coord c{x + 1, y + 1}; x + 1 < grid_size && y + 1 < grid_size && ++grid[c.x][c.y] > 9 && flashing.insert(c).second) {
			to_flash.push(c);
		}
	}

	// reset flashing to zero
	for (const auto& [x,y] : flashing) {
		grid[x][y] = 0;
	}

	return flashing.size();
}

int main() {
	std::ifstream input("input/11.txt");
	grid_t grid;
	for (std::size_t y = 0; y < grid_size; ++y) {
		for (std::size_t x = 0; x < grid_size; ++x) {
			grid[x][y] = input.get() - '0';
		}
		input.get(); // skip '\n'
	}
	std::size_t total_flashes = 0;
	std::size_t step = 0;
	for (; step < 100; ++step) {
		total_flashes += do_one_step(grid);
	}
	std::cout << "Part 1: " << total_flashes << std::endl;
	do {
		++step;
	} while(do_one_step(grid) != grid_size * grid_size);

	std::cout << "Part 2: " << step << std::endl;
}