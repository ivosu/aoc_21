#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <sstream>
#include <variant>
#include <memory>

struct snailfish_pair;

using snailfish_pair_ptr = std::unique_ptr<snailfish_pair>;

struct snailfish_pair {
	using member = std::variant<std::size_t, snailfish_pair_ptr>;

	member left;
	member right;
	snailfish_pair* parent;

	snailfish_pair(member l, member r, snailfish_pair* p = nullptr) :
		left(std::move(l)),
		right(std::move(r)),
		parent(p)
	{
		if (std::holds_alternative<snailfish_pair_ptr>(left)) {
			std::get<snailfish_pair_ptr>(left)->parent = this;
		}
		if (std::holds_alternative<snailfish_pair_ptr>(right)) {
			std::get<snailfish_pair_ptr>(right)->parent = this;
		}
	}

	snailfish_pair_ptr copy() const {
		member l;
		member r;
		if (std::holds_alternative<std::size_t>(left)) {
			l = std::get<std::size_t>(left);
		} else if (std::holds_alternative<snailfish_pair_ptr>(left)) {
			l = std::get<snailfish_pair_ptr>(left)->copy();
		}
		if (std::holds_alternative<std::size_t>(right)) {
			r = std::get<std::size_t>(right);
		} else if (std::holds_alternative<snailfish_pair_ptr>(right)) {
			r = std::get<snailfish_pair_ptr>(right)->copy();
		}
		return std::make_unique<snailfish_pair>(std::move(l), std::move(r), parent);
	}

	std::size_t& get_leftmost() {
		if (std::holds_alternative<snailfish_pair_ptr>(left)) {
			return std::get<snailfish_pair_ptr>(left)->get_leftmost();
		} else if (std::holds_alternative<std::size_t>(left)) {
			return std::get<std::size_t>(left);
		}
		throw std::runtime_error("cannot get leftmost");
	}

	std::size_t& get_rightmost() {
		if (std::holds_alternative<snailfish_pair_ptr>(right)) {
			return std::get<snailfish_pair_ptr>(right)->get_rightmost();
		} else if (std::holds_alternative<std::size_t>(right)) {
			return std::get<std::size_t>(right);
		}
		throw std::runtime_error("cannot get rightmost");
	}

	snailfish_pair_ptr operator+(const snailfish_pair& o) const {
		return std::make_unique<snailfish_pair>(this->copy(), o.copy());
	}

	bool try_explosion(std::size_t depth) {
		if (std::holds_alternative<snailfish_pair_ptr>(left)) {
			auto& lp = std::get<snailfish_pair_ptr>(left);
			if (depth == 4) {
				std::size_t ll = std::get<std::size_t>(lp->left);
				std::size_t lr = std::get<std::size_t>(lp->right);
				if (std::holds_alternative<snailfish_pair_ptr>(right)) {
					std::get<snailfish_pair_ptr>(right)->get_leftmost() += lr;
				} else {
					std::get<std::size_t>(right) += lr;
				}
				snailfish_pair* p = parent;
				snailfish_pair* current = this;
				while(p) {
					if (std::holds_alternative<std::size_t>(p->left)) {
						std::get<std::size_t>(p->left) += ll;
						break;
					} else if (auto& l = std::get<snailfish_pair_ptr>(p->left); l.get() != current) {
						l->get_rightmost() += ll;
						break;
					}
					current = p;
					p = p->parent;
				}
				left = std::size_t(0);
				return true;
			} else {
				if (lp->try_explosion(depth + 1)) {
					return true;
				}
			}
		}
		if (std::holds_alternative<snailfish_pair_ptr>(right)) {
			auto& rp = std::get<snailfish_pair_ptr>(right);
			if (depth == 4) {
				std::size_t rl = std::get<std::size_t>(rp->left);
				std::size_t rr = std::get<std::size_t>(rp->right);
				if (std::holds_alternative<snailfish_pair_ptr>(left)) {
					std::get<snailfish_pair_ptr>(left)->get_rightmost() += rl;
				} else {
					std::get<std::size_t>(left) += rl;
				}
				snailfish_pair* p = parent;
				snailfish_pair* current = this;
				while(p) {
					if (std::holds_alternative<std::size_t>(p->right)) {
						std::get<std::size_t>(p->right) += rr;
						break;
					} else if (auto& r = std::get<snailfish_pair_ptr>(p->right); r.get() != current) {
						r->get_leftmost() += rr;
						break;
					}
					current = p;
					p = p->parent;
				}
				right = std::size_t(0);
				return true;
			} else {
				if (rp->try_explosion(depth + 1)) {
					return true;
				}
			}
		}
		return false;
	}

	bool try_split() {
		if (std::holds_alternative<std::size_t>(left)) {
			std::size_t l = std::get<std::size_t>(left);
			if (l >= 10) {
				left = std::make_unique<snailfish_pair>(l/2, l-(l/2), this);
				return true;
			}
		} else {
			auto& l = std::get<snailfish_pair_ptr>(left);
			if (l->try_split()) {
				return true;
			}
		}
		if (std::holds_alternative<std::size_t>(right)) {
			std::size_t r = std::get<std::size_t>(right);
			if (r >= 10) {
				right = std::make_unique<snailfish_pair>(r/2, r-(r/2), this);
				return true;
			}
		} else {
			auto& r = std::get<snailfish_pair_ptr>(right);
			if (r->try_split()) {
				return true;
			}
		}
		return false;
	}

	void reduce() {
		while(true) {
			while (try_explosion(1));
			if (!try_split()) {
				break;
			}
		}
	}

	std::size_t calculate_magnitude() const {
		std::size_t magnitude = 0;
		if (std::holds_alternative<std::size_t>(left)) {
			magnitude += 3 * std::get<std::size_t>(left);
		} else if (std::holds_alternative<snailfish_pair_ptr>(left)) {
			magnitude += 3 * std::get<snailfish_pair_ptr>(left)->calculate_magnitude();
		}
		if (std::holds_alternative<std::size_t>(right)) {
			magnitude += 2 * std::get<std::size_t>(right);
		} else if (std::holds_alternative<snailfish_pair_ptr>(right)) {
			magnitude += 2 * std::get<snailfish_pair_ptr>(right)->calculate_magnitude();
		}
		return magnitude;
	}

	static snailfish_pair_ptr parse(std::istream& input) {
		input.get(); // '['
		snailfish_pair::member left;
		snailfish_pair::member right;
		if (input.peek() == '[') {
			left = parse(input);
		} else {
			std::size_t value;
			input >> value;
			left = value;
		}
		input.get(); // ','
		if (input.peek() == '[') {
			right = parse(input);
		} else {
			std::size_t value;
			input >> value;
			right = value;
		}
		input.get(); // ']'
		return std::make_unique<snailfish_pair>(std::move(left), std::move(right));
	}

	friend std::ostream& operator<<(std::ostream& os, const snailfish_pair& s) {
		os << '[';
		if (std::holds_alternative<std::size_t>(s.left)) {
			os << std::get<std::size_t>(s.left);
		} else {
			os << *std::get<snailfish_pair_ptr>(s.left);
		}
		os << ',';
		if (std::holds_alternative<std::size_t>(s.right)) {
			os << std::get<std::size_t>(s.right);
		} else {
			os << *std::get<snailfish_pair_ptr>(s.right);
		}
		os << ']';
		return os;
	}
};

int main() {
	std::ifstream input("input/18.txt");
	std::vector<snailfish_pair_ptr> inputs_pairs;
	std::string line;
	while(std::getline(input, line)) {
		std::stringstream ss(line);
		inputs_pairs.emplace_back(snailfish_pair::parse(ss));
	}
	if (inputs_pairs.empty()) {
		return 0;
	}
	snailfish_pair_ptr acc = inputs_pairs.front()->copy();
	for (std::size_t i = 1; i < inputs_pairs.size(); ++i) {
		acc = *acc + *inputs_pairs[i];
		acc->reduce();
	}
	std::cout << "Part 1: " << acc->calculate_magnitude() << std::endl;

	std::size_t max_magnitude = 0;
	for (std::size_t i = 0; i < inputs_pairs.size(); ++i) {
		for (std::size_t j = 0; j < inputs_pairs.size(); ++j) {
			if (i == j) {
				continue;
			}
			auto res = *inputs_pairs[i] + *inputs_pairs[j];
			res->reduce();
			max_magnitude = std::max(max_magnitude, res->calculate_magnitude());
		}
	}
	std::cout << "Part 2: " << max_magnitude << std::endl;
}