#include <fstream>
#include <iostream>
#include <functional>

#include "a_star.h"

int heuristic(spot a, spot b)
{
	return std::abs(a.index.x - b.index.x) + std::abs(a.index.y - b.index.y);
}

inline bool operator==(spot const& struct_1,  spot const& struct_2)
{
	return struct_1.index.y == struct_2.index.y && struct_1.index.x == struct_2.index.x;
}

std::vector<spot>::iterator find(spot & target, std::vector<spot> & container)
{
	return std::find(container.begin(), container.end(), target);
}

bool is_in(std::vector<spot> & container, spot & target)
{
	return (find(target, container) != container.end());
}

position index_to_coord(index & target, int size)
{
	return position{ static_cast<float>(target.x * size), static_cast<float>(target.y * size) };
}

a_star::a_star(map_infos const& infos)
{
	load_map_infos(infos);
	create_spots();
}

void a_star::load_map_infos(map_infos const& infos)
{
	nb_x_tile = infos.nb_x_tile;
	nb_y_tile = infos.nb_y_tile;
	tile_size = infos.tile_size;

	wall_map = infos.collider_map;
}

void a_star::create_spots()
{
	std::cout << "Starting create spot (A*)" << std::endl;
	for (int y{ 0 }; y < nb_y_tile; y++)
	{
		for (int x{ 0 }; x < nb_x_tile; x++)
		{
			spot_map.push_back(
				spot{ index{x, y}, 0, 0, 0, wall_map[y * nb_x_tile + x] }
			);

		}
	}

	std::cout << "Spot created (A*)" << std::endl;
}

index a_star::get_corresponding_index(float x, float y)
{
	return index{ static_cast<int>(std::floor(x / tile_size)), static_cast<int>(std::floor(y / tile_size)) };
}

std::vector<position> a_star::extract_path(std::vector<spot> & close_set)
{
	std::vector<position> path;
	spot temp{ close_set.back() };
	path.push_back(index_to_coord(temp.index, tile_size));

	while (temp.parent != -1)
	{
		path.push_back(index_to_coord(close_set[temp.parent].index, tile_size));
		temp = close_set[temp.parent];
	}

	return path;
}

std::vector<position> a_star::create_path(float x_1, float y_1, float x_2, float y_2)
{
	index b_index{ get_corresponding_index(x_1, y_1) };
	index e_index{ get_corresponding_index(x_2, y_2) };

	std::vector<spot> open_set;
	open_set.push_back(spot_map[ b_index.y * nb_y_tile + b_index.x ]);

	std::vector<spot> close_set;

	bool finish{ false };

	while (!finish)
	{
		size_t lowest_index{ 0 };
		for (size_t i{ 0 }; i < open_set.size(); i++)
		{
			if (open_set[i].f < open_set[lowest_index].f)
			{
				lowest_index = i;
			}
		}

		if (open_set[lowest_index] == spot_map[e_index.y * nb_x_tile + e_index.x])
		{
			close_set.push_back( open_set[lowest_index] );
			
			return extract_path(close_set);
		}

		close_set.push_back( open_set[lowest_index] );
		auto remove_it{ std::remove(open_set.begin(), open_set.end(), open_set[lowest_index]) };
		open_set.erase(remove_it, open_set.end());

		spot & winner{ close_set.back() };

		if (winner.index.x > 0)
		{
			spot neighbor{ spot_map[ winner.index.y * nb_x_tile + (winner.index.x - 1) ] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.index.x < nb_x_tile - 1)
		{
			spot neighbor{ spot_map[winner.index.y * nb_x_tile + (winner.index.x + 1)] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.index.y > 0)
		{
			spot neighbor{ spot_map[(winner.index.y - 1) * nb_x_tile + winner.index.x] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.index.y < nb_y_tile - 1)
		{
			spot neighbor{ spot_map[(winner.index.y + 1) * nb_x_tile + winner.index.x] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, spot_map[e_index.y * nb_x_tile + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
	}
}

a_star::~a_star()
{
}
