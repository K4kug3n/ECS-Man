#pragma once

#include <vector>
#include <string>
#include <functional>
#include <array>

#include "game_structures.h"

struct index
{
	int x;
	int y;
};

struct spot
{
	index index;
	int f;
	int h;
	int g;
	bool wall;

	int parent{ -1 };
};

class a_star
{
public:
	a_star(map_infos const& infos);

	void load_map_infos(map_infos const& infos);
	void create_spots();
	std::vector<position> create_path(float x_1, float y_1, float x_2, float y_2);

	~a_star();

private:
	index get_corresponding_index(float x, float y);
	std::vector<position> extract_path(std::vector<spot> & close_set);

	int nb_y_tile;
	int nb_x_tile;
	int tile_size;

	std::vector<bool> wall_map;
	std::vector<spot> spot_map;
};

