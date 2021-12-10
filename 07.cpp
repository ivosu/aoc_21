#include <iostream>
#include <fstream>
#include <vector>

int get_sum_distance_to(int destination, const std::vector<int>& positions)
{
	int sum = 0;
	for (int i : positions)
	{
		sum += std::abs(destination - i);
	}
	return sum;
}

int get_sum_distance_to_part_2(int destination, const std::vector<int>& positions)
{
	int sum = 0;
	for (int i : positions)
	{
		int diff = std::abs(destination - i);
		sum += (diff + 1) * (diff / 2.0);
	}
	return sum;
}

int main() {
	std::ifstream input("input/07.txt");
	int crab_position = 0;
	int min = std::numeric_limits<int>::max();
	int max = 0;
	std::vector<int> crab_positions;
	while(input >> crab_position)
	{
		min = std::min(min, crab_position);
		max = std::max(max, crab_position);
		input.get();
		crab_positions.push_back(crab_position);
	}
	int min_const_distance = std::numeric_limits<int>::max();
	for (int i = min; i < max; ++i)
	{
		min_const_distance = std::min(min_const_distance, get_sum_distance_to_part_2(i, crab_positions));
	}
	std::cout << min_const_distance << std::endl;
}