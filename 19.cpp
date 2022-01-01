#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <array>
#include <set>

template<typename T, std::size_t dimension>
std::array<T, dimension> operator+(const std::array<T, dimension>& a, const std::array<T, dimension>& b) {
	std::array<T, dimension> result{};
	for (std::size_t i = 0; i < dimension; ++i) {
		result[i] = a[i] + b[i];
	}

	return result;
}

template<typename T, std::size_t dimension>
std::array<T, dimension>& operator+=(std::array<T, dimension>& a, const std::array<T, dimension>& b) {
	for (std::size_t i = 0; i < dimension; ++i) {
		a[i] += b[i];
	}

	return a;
}

template<typename T, std::size_t dimension>
std::array<T, dimension> operator-(const std::array<T, dimension>& a, const std::array<T, dimension>& b) {
	std::array<T, dimension> result{};
	for (std::size_t i = 0; i < dimension; ++i) {
		result[i] = a[i] - b[i];
	}

	return result;
}

template<typename T, std::size_t dimension_m, std::size_t dimension_n, std::size_t dimension_p>
std::array<std::array<T, dimension_m>, dimension_n> operator*(const std::array<std::array<T, dimension_p>, dimension_n>& a, const std::array<std::array<T, dimension_m>, dimension_p>& b) {
	std::array<std::array<T, dimension_m>, dimension_n> res{};
	for (std::size_t i = 0; i < dimension_n; ++i) {
		for (std::size_t j = 0; j < dimension_m; ++j) {
			res[i][j] = 0;
			for (std::size_t k = 0; k < dimension_p; ++k) {
				res[i][j] += a[i][k] * b[k][j];
			}
 		}
	}

	return res;
}

template<typename T, std::size_t dimension_m, std::size_t dimension_n>
std::array<T, dimension_n> operator*(const std::array<T, dimension_m>& a, const std::array<std::array<T, dimension_n>, dimension_m>& b) {
	std::array<T, dimension_n> res{};
	for (std::size_t i = 0; i < dimension_n; ++i) {
		for (std::size_t j = 0; j < dimension_m; ++j) {
			res[i] += a[j] * b[j][i];
		}
	}

	return res;
}

std::array<std::array<int, 3>, 3> rotate_matrix_along_first_axis = {{
	{1, 0, 0},
	{0, 0, -1},
	{0, 1, 0},
}};

std::array<std::array<int, 3>, 3> negate_look_matrix = {{
	{-1, 0, 0},
	{0, 0, 1},
	{0, 1, 0},
}};

std::array<std::array<int, 3>, 3> shift_axis_matrix = {{
	{0, 0, 1},
	{1, 0, 0},
	{0, 1, 0},
}};

using V3 = std::array<int, 3>;

std::optional<V3> match_by_offset(const std::vector<V3>& a, const std::vector<V3>& b, std::size_t target_matching_count = 12) {
	std::set<V3> tried_offsets;
	for (std::size_t i = 0; i < a.size(); ++i) {
		for (std::size_t j = i + 1; j < b.size(); ++j) {
			V3 offset = a[i] - b[j];
			if (const auto&[it, inserted] = tried_offsets.insert(offset); inserted) {
				// already tried offset
				continue;
			}
			std::size_t matched_counter = 0;
			for (std::size_t k = 0; k < b.size() && b.size() - k + matched_counter >= target_matching_count; ++k) {
				if (std::find_if(a.begin(), a.end(), [target = b[k] + offset](const V3& i){ return i == target; }) != a.end()) {
					++matched_counter;
				}
			}
			if (matched_counter >= target_matching_count) {
				return offset;
			}
		}
	}
	return std::nullopt;
}

void transform_vector(std::vector<V3>& input, const std::array<std::array<int, 3>, 3>& transformation_matrix) {
	for (V3& x : input) {
		x = x * transformation_matrix;
	}
}

void shift_vector(std::vector<V3>& input, const std::array<int, 3>& shift) {
	for (V3& x : input) {
		x += shift;
	}
}

std::optional<std::pair<std::vector<V3>, V3>> find_transformation(const std::vector<V3>& a, const std::vector<V3>& b, std::size_t target_matching_count = 12) {
	std::vector<V3> transformed_b = b;
	for (std::size_t axis_selection_count = 0; axis_selection_count < 3; ++axis_selection_count, transform_vector(transformed_b, shift_axis_matrix)) {
		for (std::size_t rotation_selection_count = 0; rotation_selection_count < 4; ++rotation_selection_count, transform_vector(transformed_b, rotate_matrix_along_first_axis)) {
			for (std::size_t negated_cnt = 0; negated_cnt < 2; ++negated_cnt, transform_vector(transformed_b, negate_look_matrix)) {
				// try to find the offset
				if (auto opt_res = match_by_offset(a, transformed_b, target_matching_count)) {
					shift_vector(transformed_b, *opt_res);
					// return the transformed vector
					return std::make_pair(transformed_b, *opt_res);
				}
			}
		}
	}

	return std::nullopt;
}

std::pair<std::set<V3>, std::vector<V3>> get_beacons(const std::vector<std::vector<V3>>& scanner_inputs) {
	std::set<V3> beacons; // beacons in sensors 0 view/space
	std::vector<V3> sensors; // sensor positions relatively to sensor 0

	for (const V3& x : scanner_inputs[0]) {
		beacons.insert(x);
	}
	sensors.push_back({0,0,0});

	std::vector<std::vector<V3>> transformed_inputs; // transformed scanner inputs to sensor 0 view/space
	transformed_inputs.reserve(scanner_inputs.size());
	transformed_inputs.push_back(scanner_inputs[0]);


	std::vector<std::size_t> unmatched_inputs; // still unmatched input indexes
	unmatched_inputs.reserve(scanner_inputs.size() - 1);
	for (std::size_t i = 1; i < scanner_inputs.size(); ++i) {
		unmatched_inputs.push_back(i);
	}

	// helper array for knowing last checked transformed input, so they are not checked again and again
	std::vector<std::size_t> last_checked_transformed_input(scanner_inputs.size(), 0);

	while(!unmatched_inputs.empty()) {
		for (auto it = unmatched_inputs.begin(); it < unmatched_inputs.end(); ++it) {
			std::size_t unmatched_index = *it;
			for (std::size_t i = last_checked_transformed_input[unmatched_index]; i < transformed_inputs.size(); ++i) {
				auto opt_transformation = find_transformation(transformed_inputs[i], scanner_inputs[unmatched_index]);
				if (opt_transformation) {
					beacons.insert(opt_transformation->first.begin(), opt_transformation->first.end());
					sensors.push_back(opt_transformation->second);
					transformed_inputs.push_back(opt_transformation->first);
					it = unmatched_inputs.erase(it);
					break;
				}
			}
			// update the last checked index array
			last_checked_transformed_input[unmatched_index] = transformed_inputs.size();
		}
	}

	return std::make_pair(beacons, sensors);
}

int manhattan_distance(const V3& a, const V3& b) {
	return std::abs(a[0] - b[0]) + std::abs(a[1] - b[1]) + std::abs(a[2] - b[2]);
}

int main() {
	std::ifstream input("input/19.txt");
	std::vector<std::vector<V3>> scanners_inputs;
	std::string line;
	while(std::getline(input, line)) {
		// leaded the "--- scanner x ---" line
		std::vector<V3> inputs;
		while(std::getline(input, line) && !line.empty()) {
			std::istringstream iss(line);
			V3 v;
			iss >> v[0];
			iss.get();
			iss >> v[1];
			iss.get();
			iss >> v[2];
			inputs.push_back(v);
		}
		scanners_inputs.push_back(inputs);
	}

	const auto& [beacons, sensors] = get_beacons(scanners_inputs);
	std::cout << "Part 1: " << beacons.size() << std::endl;

	int max_distance = 0;
	for (const V3& i : sensors) {
		for (const V3& j : sensors) {
			if (int distance = manhattan_distance(i,j); distance > max_distance) {
				max_distance = distance;
			}
		}
	}
	std::cout << "Part 2: " << max_distance << std::endl;
}