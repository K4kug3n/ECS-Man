#pragma once

#include <vector>
#include <string>
#include <functional>
#include <array>

#include "game_structures.h"

struct spot
{
	Index spot_index;
	int f;
	int h;
	int g;
	bool wall;

	int parent{ -1 };
};

class a_star
{
public:
	a_star(Map_infos const& infos);

	void load_map_infos(Map_infos const& infos);
	void create_spots();
	std::vector<Position> create_center_path(float x_1, float y_1, float x_2, float y_2);

	~a_star();

private:
	Index get_corresponding_index(float x, float y);
	std::vector<Position> extract_path(std::vector<spot> & close_set);

	int m_nb_rows;
	int m_nb_cols;
	Size m_tile_size;

	std::vector<bool> m_wall_map;
	std::vector<spot> m_spot_map;
};
