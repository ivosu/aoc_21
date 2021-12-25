#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <charconv>
#include <numeric>
#include <stdexcept>

enum class split_method {
	lower,
	higher,
};

struct range {
	int min = 0;
	int max = 0;

	std::pair<range, range> split_by(int val, split_method split_method) const {
		if (!is_intersected_by(val)) {
			throw std::logic_error("range being split not intersected by value");
		}
		switch (split_method) {
			case split_method::lower:
				return std::make_pair<range, range>({min, val - 1}, {val, max});
			case split_method::higher:
				return std::make_pair<range, range>({min, val}, {val + 1, max});
		}

		throw std::runtime_error("unknown split method");
	}

	bool is_valid() const {
		return max >= min;
	}

	bool is_intersected_by(int val) const {
		return min <= val && max >= val;
	}

	bool contains(const range& o) const {
		return min <= o.min && max >= o.max;
	}

	std::size_t size() const {
		return max - min + 1;
	}
};

enum class dimension {
	x,
	y,
	z,
};

struct block {
	range x;
	range y;
	range z;

	bool is_valid() const {
		return x.is_valid() && y.is_valid() && z.is_valid();
	}

	std::size_t size() const {
		return x.size() * y.size() * z.size();
	}

	bool is_intersected_by(int val, dimension dim) const {
		switch (dim) {
			case dimension::x:
				return x.is_intersected_by(val);
			case dimension::y:
				return y.is_intersected_by(val);
			case dimension::z:
				return z.is_intersected_by(val);
		}

		throw std::runtime_error("unknown dimension");
	}

	bool contains(const block& o) const {
		return x.contains(o.x) && y.contains(o.y) && z.contains(o.z);
	}

	bool intersects_with(const block& o) const {
		return (x.min <= o.x.max && x.max >= o.x.min) &&
				 (y.min <= o.y.max && y.max >= o.y.min) &&
				 (z.min <= o.z.max && z.max >= o.z.min);
	}

	std::pair<block, block> split_by(int val, dimension dim, split_method split_method) const {
		switch (dim) {
			case dimension::x:
			{
				std::pair<range, range> new_xs = x.split_by(val, split_method);
				return std::make_pair<block, block>({new_xs.first, y, z}, {new_xs.second, y, z});
			}
			case dimension::y:
			{
				std::pair<range, range> new_ys = y.split_by(val, split_method);
				return std::make_pair<block, block>({x, new_ys.first, z}, {x, new_ys.second, z});
			}
			case dimension::z:
			{
				std::pair<range, range> new_zs = z.split_by(val, split_method);
				return std::make_pair<block, block>({x, y, new_zs.first}, {x, y, new_zs.second});
			}
		}

		throw std::runtime_error("unknown dimension");
	}
};

struct robot_step {
	enum class action {
		on,
		off,
	};

	action action = action::on;

	block area;
};



std::vector<block> split_by(const std::vector<block>& blocks, int val, dimension dim, split_method split_method) {
	std::vector<block> res;
	res.reserve(blocks.size() * 2);
	for (const block& b : blocks) {
		if (b.is_intersected_by(val, dim)) {
			std::pair<block, block> split = b.split_by(val, dim, split_method);
			if (split.first.is_valid()) {
				res.push_back(split.first);
			}
			if (split.second.is_valid()) {
				res.push_back(split.second);
			}
		} else {
			res.push_back(b);
		}
	}

	return res;
}

std::vector<block> split_by(const std::vector<block>& blocks, const block& block) {
	std::vector<class block> res;
	res.reserve(blocks.size() * 8);
	for (const class block& b : blocks) {
		if (b.intersects_with(block)) {
			auto v =
				split_by(
					split_by(
						split_by(
							split_by(
								split_by(
									split_by(
										{b},
										block.x.min,
										dimension::x,
										split_method::lower
									),
									block.x.max,
									dimension::x,
									split_method::higher
								),
								block.y.min,
								dimension::y,
								split_method::lower
							),
							block.y.max,
							dimension::y,
							split_method::higher
						),
						block.z.min,
						dimension::z,
						split_method::lower
					),
					block.z.max,
					dimension::z,
					split_method::higher
				);
			res.insert(res.end(), v.begin(), v.end());
		} else {
			res.push_back(b);
		}
	}
	return res;
}

int parse_int(const std::string& str) {
	int tmp;
	std::from_chars(str.data(), str.data() + str.size(), tmp);
	return tmp;
}

std::size_t calculate_final_on_lights(const std::vector<robot_step>& steps) {
	std::vector<block> on_blocks;

	for (const robot_step& s: steps) {
		std::vector<block> new_on_blocks;
		std::vector<block> split_blocks = split_by(on_blocks, s.area);
		new_on_blocks.reserve(split_blocks.size());
		for (const block& new_block : split_blocks) {
			if (s.area.contains(new_block)) {
				continue;
			}
			new_on_blocks.push_back(new_block);
		}
		if (s.action == robot_step::action::on) {
			new_on_blocks.push_back(s.area);
		}
		on_blocks = new_on_blocks;
	}

	return std::accumulate(on_blocks.begin(), on_blocks.end(), std::size_t(0), [](std::size_t a, const block& b) { return a + b.size(); });
}

int main() {
	std::ifstream input("input/22.txt");
	// prepare regex
	std::regex r(R"((on|off) x=(-?\d*)..(-?\d*),y=(-?\d*)..(-?\d*),z=(-?\d*)..(-?\d*))");

	auto is_part_1_range = [](const robot_step& step) {
		return step.area.x.min >= -50 && step.area.x.max <= 50 && step.area.y.min >= -50 && step.area.y.max <= 50 && step.area.z.min >= -50 && step.area.z.max <= 50;
	};

	std::vector<robot_step> steps;
	std::vector<robot_step> part_1_steps;
	std::string line;
	while(std::getline(input, line)) {
		std::smatch match;
		std::regex_match(line, match, r);
		robot_step step;
		if (match[1] == "on") {
			step.action = robot_step::action::on;
		} else if (match[1] == "off") {
			step.action = robot_step::action::off;
		}
		step.area.x.min = parse_int(match[2].str());
		step.area.x.max = parse_int(match[3].str());
		step.area.y.min = parse_int(match[4].str());
		step.area.y.max = parse_int(match[5].str());
		step.area.z.min = parse_int(match[6].str());
		step.area.z.max = parse_int(match[7].str());
		steps.push_back(step);
		if (is_part_1_range(step)) {
			part_1_steps.push_back(step);
		}
	}

	std::cout << "Part 1: " << calculate_final_on_lights(part_1_steps) << std::endl;
	std::cout << "Part 2: " << calculate_final_on_lights(steps) << std::endl;
}