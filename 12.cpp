#include <iostream>
#include <fstream>
#include <vector>
#include <optional>
#include <unordered_map>
#include <forward_list>

bool is_small_cave(const std::string& cave_name) {
	return cave_name.empty() || std::islower(cave_name.front());
}

struct cave {
	explicit cave(const std::string& cave_name) : name(cave_name), is_small(is_small_cave(cave_name))
	{}

	bool operator<(const cave& o) const {
		return name < o.name;
	}

	bool operator==(const cave& o) const {
		return name == o.name;
	}

	std::string name;
	bool is_small;
};

template<>
struct std::hash<cave> {
	std::size_t operator()(const cave& c) const {
		return std::hash<std::string>()(c.name);
	}
};

using graph_t = std::unordered_map<cave, std::vector<cave>>;

class visited_caves_list {
public:
	visited_caves_list(const cave& value) : m_value(value), m_next(std::nullopt)
	{}

	visited_caves_list(const cave& value, const visited_caves_list& next) : m_value(value), m_next(&next)
	{}

	bool contains(const cave& key) const {
		if (m_value == key) {
			return true;
		} else if (m_next) {
			return m_next.value()->contains(key);
		}
		return false;
	}

private:
	cave m_value;
	std::optional<const visited_caves_list*> m_next;
};

std::size_t get_route_counts_part_1(const graph_t& graph, const cave& from, const visited_caves_list& already_visited_small) {
	if (from.name == "end") {
		return 1;
	}
	std::size_t count = 0;
	for (const cave& c : graph.at(from)) {
		if (c.is_small) {
			if (already_visited_small.contains(c)) {
				// already visited small cave, skip
				continue;
			}
			// add this cave to visited small caves
			count += get_route_counts_part_1(graph, c, visited_caves_list(c, already_visited_small));
		} else {
			// big cave
			count += get_route_counts_part_1(graph, c, already_visited_small);
		}
	}
	return count;
}

std::size_t get_route_counts_part_2(const graph_t& graph, const cave& from, const visited_caves_list& already_visited_small) {
	if (from.name == "end") {
		return 1;
	}
	std::size_t count = 0;
	for (const cave& c : graph.at(from)) {
		if (c.is_small) {
			if (already_visited_small.contains(c)) {
				// don't allow second entrance of start
				if (c.name != "start") {
					// second enter this one, continue with the normal rules as in part 1
					count += get_route_counts_part_1(graph, c, already_visited_small);
				}
			} else {
				// add this cave to visited small caves
				count += get_route_counts_part_2(graph, c, visited_caves_list(c, already_visited_small));
			}
		} else {
			// big cave
			count += get_route_counts_part_2(graph, c, already_visited_small);
		}
	}
	return count;
}

int main() {
	std::ifstream input("input/12.txt");
	graph_t graph;
	while(input) {
		std::string from;
		std::string to;
		for (char c = input.get(); c != '-'; c = input.get()) {
			from += c;
		}
		for (char c = input.get(); c != '\n' && c != EOF; c = input.get()) {
			to += c;
		}
		cave from_cave(from);
		cave to_cave(to);
		graph[from_cave].push_back(to_cave);
		graph[to_cave].push_back(from_cave);
	}
	cave start_cave("start");
	std::cout << "Part 1: " << get_route_counts_part_1(graph, start_cave, { start_cave }) << std::endl;
	std::cout << "Part 2: " << get_route_counts_part_2(graph, start_cave, { start_cave }) << std::endl;
}