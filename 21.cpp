#include <iostream>
#include <fstream>
#include <string>
#include <functional>
#include <map>
#include <sstream>

struct game_state {
	game_state(std::size_t player_1_pos, std::size_t player_2_pos) noexcept :
		player_1_position(player_1_pos),
		player_2_position(player_2_pos)
	{}

	std::size_t player_1_position;
	std::size_t player_1_score = 0;
	std::size_t player_2_position;
	std::size_t player_2_score = 0;
	bool player_1_next = true;

	bool operator==(const game_state& o) const noexcept {
		return std::tie(
			player_1_position, player_1_score,
			player_2_position, player_2_score,
			player_1_next
		)
			==
		std::tie(
			o.player_1_position, o.player_1_score,
			o.player_2_position, o.player_2_score,
			o.player_1_next
		);
	}

	bool operator<(const game_state& o) const noexcept {
		return std::tie(
			player_1_position, player_1_score,
			player_2_position, player_2_score,
			player_1_next
		)
			<
		std::tie(
			o.player_1_position, o.player_1_score,
			o.player_2_position, o.player_2_score,
			o.player_1_next
		);
	}
};

template<>
struct std::hash<game_state> {
	std::size_t operator()(const game_state& game_state) const noexcept {
		std::size_t seed = 0;
		seed ^= std::hash<std::size_t>{}(game_state.player_1_score) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<std::size_t>{}(game_state.player_1_position) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<std::size_t>{}(game_state.player_2_score) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<std::size_t>{}(game_state.player_2_position) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= std::hash<bool>{}(game_state.player_1_next) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	}
};

struct game_outcome {
	using counter_t = std::size_t;
	counter_t player_1_won_games;
	counter_t player_2_won_games;
};

void advance_game(game_state& game_state, std::size_t die_throws_sum) noexcept {
	std::size_t& current_player_score = (game_state.player_1_next ? game_state.player_1_score : game_state.player_2_score);
	std::size_t& current_player_pos = (game_state.player_1_next ? game_state.player_1_position : game_state.player_2_position);
	current_player_pos += die_throws_sum;
	current_player_pos = ((current_player_pos - 1) % 10) + 1;
	current_player_score += current_player_pos;
	game_state.player_1_next = !game_state.player_1_next;
}

game_state next_state(const game_state& game_state, std::size_t die_throws_sum) noexcept {
	struct game_state next_game_state(game_state);
	advance_game(next_game_state, die_throws_sum);
	return next_game_state;
}

const game_outcome& calculate_games(std::unordered_map<game_state, game_outcome>& outcomes, const game_state& game_state) noexcept {
	if (auto it = outcomes.find(game_state); it != outcomes.end()) {
		return it->second;
	}
	if (game_state.player_1_score >= 21) {
		return outcomes[game_state] = {1, 0};
	}
	if (game_state.player_2_score >= 21) {
		return outcomes[game_state] = {0, 1};
	}
	game_outcome::counter_t player_1_won_games = 0;
	game_outcome::counter_t player_2_won_games = 0;
	for (std::size_t first_roll = 1; first_roll <= 3; ++first_roll) {
		for (std::size_t second_roll = 1; second_roll <= 3; ++second_roll) {
			for (std::size_t third_roll = 1; third_roll <= 3; ++third_roll) {
				auto res = calculate_games(outcomes, next_state(game_state, first_roll + second_roll + third_roll));
				player_1_won_games += res.player_1_won_games;
				player_2_won_games += res.player_2_won_games;
			}
		}
	}
	return outcomes[game_state] = {player_1_won_games, player_2_won_games};
}

int main() {
	std::ifstream input("input/21.txt");

	std::size_t player_1_pos;
	std::size_t player_2_pos;

	{
		std::string player_1_pos_line;
		std::getline(input, player_1_pos_line);
		std::stringstream ss(player_1_pos_line.substr(28));
		ss >> player_1_pos;
	}
	{
		std::string player_2_pos_line;
		std::getline(input, player_2_pos_line);
		std::stringstream ss(player_2_pos_line.substr(28));
		ss >> player_2_pos;
	}
	{
		game_state game(player_1_pos, player_2_pos);
		std::size_t dice_rolled = 0;
		std::size_t next_die_roll = 1;
		while (game.player_1_score < 1000 && game.player_2_score < 1000) {
			advance_game(game, 3 * next_die_roll + 3);
			dice_rolled += 3;
			next_die_roll += 3;
		}
		std::cout << "Part 1: " << dice_rolled * std::min(game.player_1_score, game.player_2_score) << std::endl;
	}
	{
		std::unordered_map<game_state, game_outcome> outcomes;
		game_state game(player_1_pos, player_2_pos);
		auto res = calculate_games(outcomes, game);
		std::cout << "Part 2: " << std::max(res.player_1_won_games, res.player_2_won_games) << std::endl;
	}
}