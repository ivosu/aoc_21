#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <limits>
#include <map>

using pixel_t = bool;

class enhance_algorithm {
public:
	using table_t = std::array<pixel_t, 512>;

	enhance_algorithm(const std::string& raw_table) {
		for (std::size_t i = 0; i < raw_table.size(); ++i) {
			table_[i] = raw_table[i] == '#';
		}
	}

	pixel_t get_enhanced(const std::array<pixel_t, 9>& input) const {
		std::size_t index = 0;
		for (pixel_t i : input) {
			index = (index << 1) + (i ? 1 : 0);
		}
		return table_.at(index);
	}
private:
	table_t table_;
};

class image {
public:
	image(std::vector<std::vector<pixel_t>> base_image) : width_(base_image.size()), height_(base_image.front().size()), outside_pixels_(false),
		image_(width_, std::vector<pixel_t>(height_, false)){
		for (int i = 0; i < width_; ++i) {
			for (int j = 0; j < height_; ++j) {
				image_[i][j] = base_image[i][j];
			}
		}
	}

	std::size_t count_light_pixels() const {
		if (outside_pixels_) {
			return std::numeric_limits<std::size_t>::max();
		}

		std::size_t counter = 0;
		for (int i = 0; i < width_; ++i) {
			for (int j = 0; j < height_; ++j) {
				if (image_[i][j]) {
					++counter;
				}
			}
		}

		return counter;
	}

	void enhance(const enhance_algorithm& enhance_algorithm) {
		std::vector<std::vector<pixel_t>> new_image(width_ + 2, std::vector<pixel_t>(height_ + 2, false));
		// calculate new "infinite pixels" value
		std::array<pixel_t, 9> input = {
			outside_pixels_, outside_pixels_, outside_pixels_,
			outside_pixels_, outside_pixels_, outside_pixels_,
			outside_pixels_, outside_pixels_, outside_pixels_,
		};
		pixel_t new_outside_pixels = enhance_algorithm.get_enhanced(input);

		for (int x = -1; x <= width_; ++x) {
			// load new input matrix
			input = {
				get_pixel(x - 1, -2), get_pixel(x - 1, -1), get_pixel(x - 1, 0),
				get_pixel(x,     -2), get_pixel(x,     -1), get_pixel(x,     0),
				get_pixel(x + 1, -2), get_pixel(x + 1, -1), get_pixel(x + 1, 0),
			};
			for (int y = -1; y < height_; ++y) {
				new_image[x + 1][y + 1] = enhance_algorithm.get_enhanced(input);
				// shift right
				input[0] = input[1];
				input[3] = input[4];
				input[4] = input[5];
				input[1] = input[2];
				input[6] = input[7];
				input[7] = input[8];
				// load new pixels from right
				input[2] = get_pixel(x - 1, y + 2);
				input[5] = get_pixel(x, y + 2);
				input[8] = get_pixel(x + 1, y + 2);
			}
			new_image[x + 1][height_ + 1] = enhance_algorithm.get_enhanced(input);
		}

		width_ += 2;
		height_ += 2;
		image_ = new_image;
		outside_pixels_ = new_outside_pixels;
	}

private:
	pixel_t get_pixel(int x, int y) const {
		if (x < 0 || x >= width_ || y < 0 || y >= height_) {
			return outside_pixels_;
		}
		return image_[x][y];
	}

	int width_;
	int height_;
	pixel_t outside_pixels_;
	std::vector<std::vector<pixel_t>> image_;
};

int main() {
	std::ifstream input("input/20.txt");
	std::string raw_enhance_table;
	std::getline(input, raw_enhance_table);
	input.get(); // skip the empty lineW
	std::vector<std::vector<pixel_t>> raw_image;
	std::string image_line;
	while(std::getline(input, image_line)) {
		raw_image.emplace_back(image_line.size(), false);
		for (std::size_t i = 0; i < image_line.size(); ++i) {
			if (image_line[i] == '#') {
				raw_image.back()[i] = true;
			}
		}
	}
	enhance_algorithm alg(raw_enhance_table);

	image img(std::move(raw_image));
	img.enhance(alg);
	img.enhance(alg);

	std::cout << "Part 1: " << img.count_light_pixels() << std::endl;

	for (std::size_t i = 2; i < 50; ++i) {
		img.enhance(alg);
	}
	std::cout << "Part 2: " << img.count_light_pixels() << std::endl;
}