#pragma once

#include <vector>
#include <string>

struct spot
{
	int x;
	int y;
	float f;
	float h;
	float g;
	bool wall;
	std::vector<spot> neighbors;
	spot * parent = nullptr;
};

class a_star
{
public:
	a_star(std::string const& file_path);

	void load_map(std::string const& file_path);
	void create_spots();
	void create_path(spot & begin, spot & end);

	~a_star();

private:
	void add_neighbors();

	int nb_y_tile;
	int nb_x_tile;
	int tile_size;

	std::vector<bool> wall_map;
	std::vector<spot> spot_map;
};

