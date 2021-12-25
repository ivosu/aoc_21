#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <numeric>
#include <algorithm>

class packet {
public:
	static std::unique_ptr<packet> parse_packet(std::vector<bool>::iterator& it);

	packet(int version, int type_id, std::size_t bit_length) : m_version(version), m_type_id(type_id), m_bit_length(bit_length) {}

	int version() const {
		return m_version;
	}

	int type_id() const {
		return m_type_id;
	}

	std::size_t bit_length() const {
		return m_bit_length;
	}

	virtual std::size_t sum_versions() const = 0;

private:
	int m_version;
	int m_type_id;
	std::size_t m_bit_length;
};

class literal_value_packet : public packet {
public:
	static std::unique_ptr<literal_value_packet> parse_literal_value_packet(std::vector<bool>::iterator& it, int version, int type_id) {
		bool last;
		std::size_t value = 0;
		std::size_t bit_length = 0;
		do {
			last = !*it++;
			for (std::size_t i = 0; i < 4; ++i) {
				value <<= 1;
				value |= (*it++ ? 1 : 0);
			}
				bit_length += 5;
		} while(!last);

		return std::make_unique<literal_value_packet>(version, type_id, value, bit_length);
	}

	literal_value_packet(int version, int type_id, std::size_t value, std::size_t value_bit_length) : packet(version, type_id, 6 + value_bit_length), m_value(value) {}

	std::size_t value() const {
		return m_value;
	}

	std::size_t sum_versions() const override {
		return version();
	}

private:
	std::size_t m_value;
};

class operator_packet : public packet {
public:
	static std::unique_ptr<operator_packet> parse_operator_packet(std::vector<bool>::iterator& it, int version, int type_id) {
		std::vector<std::unique_ptr<packet>> subpackets;
		std::size_t bit_length = 0;

		if (*it++) {
			bit_length += 11;
			std::size_t subpackets_count = 0;
			for (std::size_t i = 0; i < 11; ++i) {
				subpackets_count <<= 1;
				subpackets_count |= (*it++ ? 1 : 0);
			}
			subpackets.reserve(subpackets_count);
			for(std::size_t i = 0; i < subpackets_count; ++i) {
				subpackets.push_back(packet::parse_packet(it));
				bit_length += subpackets.back()->bit_length();
			}
		} else {
			bit_length += 15;
			std::size_t total_bit_length = 0;
			for (std::size_t i = 0; i < 15; ++i) {
				total_bit_length <<= 1;
				total_bit_length |= (*it++ ? 1 : 0);
			}
			bit_length += total_bit_length;
			std::size_t subpackets_bit_length = 0;
			while(subpackets_bit_length < total_bit_length) {
				subpackets.push_back(packet::parse_packet(it));
				subpackets_bit_length += subpackets.back()->bit_length();
			}
		}

		return std::make_unique<operator_packet>(version, type_id, std::move(subpackets), bit_length);
	}

	operator_packet(int version, int type_id, std::vector<std::unique_ptr<packet>> subpackets, std::size_t subpackets_bit_length) :
		packet(version, type_id, 7 + subpackets_bit_length),
		m_subpackets(std::move(subpackets))
	{}

	const std::vector<std::unique_ptr<packet>>& subpackets() const {
		return m_subpackets;
	}

	std::size_t sum_versions() const override {
		std::size_t subpackets_versions = 0;
		for (const auto& p : m_subpackets) {
			subpackets_versions += p->sum_versions();
		}

		return version() + subpackets_versions;
	}
private:
	std::vector<std::unique_ptr<packet>> m_subpackets;
};

std::unique_ptr<packet> packet::parse_packet(std::vector<bool>::iterator& it) {
	int version = 0;
	int type_id = 0;
	for (std::size_t i = 0; i < 3; ++i) {
		version <<= 1;
		version |= (*it++ ? 1 : 0);
	}
	for (std::size_t i = 0; i < 3; ++i) {
		type_id <<= 1;
		type_id |= (*it++ ? 1 : 0);
	}
	if (type_id == 4) {
		return literal_value_packet::parse_literal_value_packet(it, version, type_id);
	} else {
		return operator_packet::parse_operator_packet(it, version, type_id);
	}
}

std::size_t evaluate_packet(const std::unique_ptr<packet>& packet) {
	if (packet->type_id() == 4) {
		return dynamic_cast<literal_value_packet*>(packet.get())->value();
	} else {
		auto packet_as_operator = dynamic_cast<operator_packet*>(packet.get());
		std::vector<std::size_t> evaluated_subpackets;
		evaluated_subpackets.reserve(packet_as_operator->subpackets().size());
		for (const auto& p : packet_as_operator->subpackets()) {
			evaluated_subpackets.push_back(evaluate_packet(p));
		}
		switch(packet_as_operator->type_id()) {
			case 0:
				return std::accumulate(evaluated_subpackets.begin(), evaluated_subpackets.end(), 0ull);
			case 1:
				return std::accumulate(evaluated_subpackets.begin(), evaluated_subpackets.end(), 1ull, std::multiplies());
			case 2:
				return *std::min_element(evaluated_subpackets.begin(), evaluated_subpackets.end());
			case 3:
				return *std::max_element(evaluated_subpackets.begin(), evaluated_subpackets.end());
			case 5:
				if (evaluated_subpackets.front() > evaluated_subpackets.back()) {
					return 1;
				} else {
					return 0;
				}
			case 6:
				if (evaluated_subpackets.front() < evaluated_subpackets.back()) {
					return 1;
				} else {
					return 0;
				}
			case 7:
				if (evaluated_subpackets.front() == evaluated_subpackets.back()) {
					return 1;
				} else {
					return 0;
				}
		}
		return 0;
	}
}

int main() {
	std::ifstream input("input/16.txt");
	std::vector<bool> decoded_input;
	std::string line;
	std::getline(input, line);
	decoded_input.reserve(line.size() * 4);
	for (char c : line) {
		if (c >= '0' && c <= '9') {
			c -= '0';
		} else {
			c -= 'A' - 10;
		}
		for (std::size_t i = 0; i < 4; ++i) {
			decoded_input.push_back((c >> (3 - i)) & 1);
		}
	}

	auto it = decoded_input.begin();
	auto packet = packet::parse_packet(it);
	std::cout << "Part 1: " << packet->sum_versions() << std::endl;
	std::cout << "Part 2: " << evaluate_packet(packet) << std::endl;
}