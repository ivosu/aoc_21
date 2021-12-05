#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <fstream>
#include <sstream>

constexpr std::size_t board_size = 5;

using raw_board_t = std::array<std::array<int, board_size>, board_size>;

class bingo_board {
public:
	bingo_board(const raw_board_t& board) {
		for (std::size_t x = 0; x < board_size; ++x) {
			m_checked_board[x].fill(false);
			for (std::size_t y = 0; y < board_size; ++y) {
				m_numbers[board[x][y]] = {x, y};
			}
		}
	}

	std::optional<int> next_number(int number) {
		auto opt_hit = check_number(number);
		if (!opt_hit) {
			return std::nullopt;
		}
		return check_for_win(opt_hit.value());
	}

	bool is_won() const {
		return m_is_won;
	}

private:
	bool m_is_won = false;

	std::optional<std::pair<std::size_t, std::size_t>> check_number(int number) {
		auto it = m_numbers.find(number);
		if ( it == m_numbers.end()) {
			return std::nullopt;
		}
		std::size_t x = it->second.first;
		std::size_t y = it->second.second;
		m_checked_board[x][y] = true;
		m_numbers.erase(number);
		return std::make_pair(x, y);
	}

	bool check_row_filled(std::size_t row) {
		return std::all_of(m_checked_board[row].begin(), m_checked_board[row].end(), [](bool x) { return x; });
	}

	bool check_column_filled(std::size_t column) {
		for (std::size_t i = 0; i < board_size; ++i) {
			if (!m_checked_board[i][column]) {
				return false;
			}
		}

		return true;
	}

	std::optional<int> check_for_win(const std::pair<std::size_t, std::size_t>& last_hit) {
		if (!check_row_filled(last_hit.first) && !check_column_filled(last_hit.second)) {
			return std::nullopt;
		}
		m_is_won = true;
		int sum = 0;
		for (const auto& [num, _] : m_numbers) {
			sum += num;
		}
		return sum;
	}

	std::map<int, std::pair<std::size_t, std::size_t>> m_numbers;
	// rows first, then columns
	std::array<std::array<bool, board_size>, board_size> m_checked_board;
};

int main() {
	std::ifstream input("input/04.txt");
	// parse numbers
	std::string numbers_line;
	std::getline(input, numbers_line);
	std::stringstream numbers_stream(numbers_line);
	std::vector<int> numbers;
	int number;
	while(numbers_stream >> number) {
		numbers.push_back(number);
		// skip possible ','
		numbers_stream.get();
	}
	// start parsing boards
	std::vector<bingo_board> boards;
	while(input) {
		raw_board_t board;
		for (std::size_t x = 0; x < board_size; ++x) {
			for (std::size_t y = 0; y < board_size; ++y) {
				input >> board[x][y];
			}
		}
		boards.emplace_back(board);
	}
	// run the game
	for (int current_number : numbers) {
		for (bingo_board& board : boards) {
			if (board.is_won()) {
				continue;
			}
			if (auto res = board.next_number(current_number)) {
				// print all of them
				// for part 1 inset first one, for part 2 inset last one
				std::cout << res.value() * current_number << std::endl;
			}
		}
	}
}