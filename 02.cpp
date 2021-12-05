#include <iostream>
#include <fstream>

int main() {
	std::ifstream input("input/02.txt");
	int x = 0;
	int y = 0;
	int aim = 0;
	std::string dir;
	int diff;
	while(input >> dir >> diff) {
		if (dir == "forward") {
			x += diff;
			y += aim * diff; // comment for part 1
		}
		if (dir == "down") {
			// y += diff; // uncomment for part 1
			aim += diff;
		}
		if (dir == "up") {
			// y -= diff; // uncomment for part 1
			aim -= diff;
		}
	}
	std::cout << x * y << std::endl;
}