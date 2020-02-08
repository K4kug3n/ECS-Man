#include <fstream>
#include <iostream>
#include <functional>
#include <cmath>
#include <algorithm>

#include "a_star.h"

int heuristic(spot a, spot b)
{
	return std::abs(a.spot_index.x - b.spot_index.x) + std::abs(a.spot_index.y - b.spot_index.y);
}

inline bool operator==(spot const& struct_1,  spot const& struct_2)
{
	return struct_1.spot_index.y == struct_2.spot_index.y && struct_1.spot_index.x == struct_2.spot_index.x;
}

std::vector<spot>::iterator find(spot & target, std::vector<spot> & container)
{
	return std::find(container.begin(), container.end(), target);
}

bool is_in(std::vector<spot> & container, spot & target)
{
	return (find(target, container) != container.end());
}

Position index_to_coord(Index const& target, Size const& Size)
{
	return Position{ static_cast<float>(target.x * Size.width), static_cast<float>(target.y * Size.height) };
}

a_star::a_star(Map_infos const& infos)
{
	load_map_infos(infos);
	create_spots();
}

void a_star::load_map_infos(Map_infos const& infos)
{
	m_nb_cols = infos.nb_cols;
	m_nb_rows = infos.nb_rows;
	m_tile_size = infos.tile_size;

	m_wall_map = infos.collider_map;
}

void a_star::create_spots()
{
	std::cout << "Starting create spot (A*)" << std::endl;
	for (int y{ 0 }; y < m_nb_rows; y++)
	{
		for (int x{ 0 }; x < m_nb_cols; x++)
		{
			m_spot_map.push_back(
				spot{ Index{x, y}, 0, 0, 0, m_wall_map[y * m_nb_cols + x] }
			);

		}
	}

	std::cout << "Spot created (A*)" << std::endl;
}

Index a_star::get_corresponding_index(float x, float y)
{
	return Index{ static_cast<int>(std::floor(x / m_tile_size.width)), static_cast<int>(std::floor(y / m_tile_size.height)) };
}

std::vector<Position> a_star::extract_path(std::vector<spot> & close_set)
{
	std::vector<Position> path;
	spot temp{ close_set.back() };
	path.push_back( get_center(index_to_coord(temp.spot_index, m_tile_size), m_tile_size) );

	while (temp.parent != -1)
	{
		path.push_back( get_center(index_to_coord(close_set[temp.parent].spot_index, m_tile_size), m_tile_size) );
		temp = close_set[temp.parent];
	}

	return path;
}

std::vector<Position> a_star::create_center_path(float x_1, float y_1, float x_2, float y_2)
{
	Index b_index{ get_corresponding_index(x_1, y_1) };
	Index e_index{ get_corresponding_index(x_2, y_2) };

	std::vector<spot> open_set;
	open_set.push_back(m_spot_map[ b_index.y * m_nb_cols + b_index.x ]);

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

		if (open_set[lowest_index] == m_spot_map[e_index.y * m_nb_cols + e_index.x])
		{
			close_set.push_back( open_set[lowest_index] );

			return extract_path(close_set);
		}

		close_set.push_back( open_set[lowest_index] );
		auto remove_it{ std::remove(open_set.begin(), open_set.end(), open_set[lowest_index]) };
		open_set.erase(remove_it, open_set.end());

		spot & winner{ close_set.back() };

		if (winner.spot_index.x > 0)
		{
			spot neighbor{ m_spot_map[ winner.spot_index.y * m_nb_cols + (winner.spot_index.x - 1) ] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.spot_index.x < m_nb_cols - 1)
		{
			spot neighbor{ m_spot_map[winner.spot_index.y * m_nb_cols + (winner.spot_index.x + 1)] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.spot_index.y > 0)
		{
			spot neighbor{ m_spot_map[(winner.spot_index.y - 1) * m_nb_cols + winner.spot_index.x] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor.f = neighbor.h + neighbor.g;
					neighbor.parent = close_set.size() - 1;
					open_set.push_back(neighbor);
				}

			}
		}
		if (winner.spot_index.y < m_nb_rows - 1)
		{
			spot neighbor{ m_spot_map[(winner.spot_index.y + 1) * m_nb_cols + winner.spot_index.x] };
			if (!is_in(close_set, neighbor) && !neighbor.wall)
			{
				int g_temp{ winner.g + 1 };
				if (is_in(open_set, neighbor))
				{
					spot & neighbor_close = *find(neighbor, open_set); //CARE ABOUT THIS, REMPLACE ALL
					if (g_temp < neighbor_close.g) { neighbor_close.g = g_temp; };

					neighbor_close.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
					neighbor_close.f = neighbor.h + neighbor.g;
					neighbor_close.parent = close_set.size() - 1;
				}
				else
				{
					neighbor.g = g_temp;
					neighbor.h = heuristic(neighbor, m_spot_map[e_index.y * m_nb_cols + e_index.x]);
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
