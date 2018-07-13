#pragma once

#include <vector>

struct map_infos
{
	int nb_x_tile;
	int nb_y_tile;
	int tile_size;
	std::vector<bool> collider_map;
	std::vector<size_t> id_map;
};

struct position
{
	float x;
	float y;
};