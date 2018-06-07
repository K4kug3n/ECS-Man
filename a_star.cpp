#include "a_star.h"

#include <fstream>
#include <iostream>

float heuristic(spot a, spot b)
{
	return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

inline bool operator==(const spot& struct_1, const spot& struct_2)
{
	return struct_1.x == struct_2.x && struct_1.y == struct_2.y;
}

a_star::a_star(std::string const& file_path)
{
	load_map(file_path);
	create_spots();
}

void a_star::load_map(std::string const & file_path)
{
	std::fstream flux{ file_path };

	flux >> nb_x_tile;
	flux >> nb_y_tile;
	flux >> tile_size;

	bool temp;
	for (auto i{ 0 }; i < nb_x_tile * nb_y_tile; i++)
	{
		flux >> temp;
		wall_map.push_back(temp);
	}

}

void a_star::create_spots()
{
	for (int y{ 0 }; y < nb_y_tile; y++)
	{
		for (int x{ 0 }; x < nb_x_tile; x++)
		{

			spot_map.push_back(
				spot{ x*tile_size, y*tile_size, 0, 0, 0, wall_map[y * nb_x_tile + x] }
			);

		}
	}

	add_neighbors();
}

void a_star::add_neighbors()
{
	for (int y{ 0 }; y < nb_y_tile; y++)
	{
		for (int x{ 0 }; x < nb_x_tile; x++)
		{
			if (spot_map[y * nb_x_tile + x].x > 0)
			{
				spot_map[y * nb_x_tile + x].neighbors.push_back( spot_map[y * nb_x_tile + (x - 1)] );
			}
			if (spot_map[y * nb_x_tile + x].x < nb_x_tile * tile_size)
			{
				spot_map[y * nb_x_tile + x].neighbors.push_back(spot_map[y * nb_x_tile + (x + 1)]);
			}
			if (spot_map[y * nb_x_tile + x].y > 0)
			{
				spot_map[y * nb_x_tile + x].neighbors.push_back(spot_map[(y - 1) * nb_x_tile + x]);
			}
			if (spot_map[y * nb_x_tile + x].y < nb_y_tile * tile_size)
			{
				spot_map[y * nb_x_tile + x].neighbors.push_back(spot_map[(y + 1) * nb_x_tile + (x - 1)]);
			}
		}
	}
}

void a_star::create_path(spot & begin, spot & end)
{
	std::vector<spot> open_set;
	std::vector<spot> close_set;

	bool finish{ false };

	while (!finish)
	{
		if (!open_set.empty())
		{
			size_t low_index { 0 };

			for (auto i{ 0 }; i < open_set.size(); i++)
			{
				if (open_set[i].f < open_set[low_index].f)
				{
					low_index = i;
				}
			}

			if (open_set[low_index] == end)
			{
				std::cout << "Finsih !" << std::endl;

				std::vector<spot> path;
				spot temp{ open_set[low_index] };
				while (temp.parent != nullptr)
				{
					path.push_back(*temp.parent);
					temp = *temp.parent;
				}
			}

			spot & winner{ open_set[low_index] };

			close_set.push_back(winner);
			auto winner_it{ std::find(open_set.begin(), open_set.end(), winner) };
			open_set.erase(winner_it, open_set.end());

			for (size_t i{ 0 }; i < winner.neighbors.size(); i++)
			{
				if (std::find(close_set.begin(), close_set.end(), winner.neighbors[i]) == close_set.end())
				{
					float g_temp{ winner.g + 1 };
					if (std::find(open_set.begin(), open_set.end(), winner.neighbors[i]) != open_set.end())
					{
						if (g_temp < winner.neighbors[i].g) { winner.neighbors[i].g = g_temp; }
					}
					else 
					{
						winner.neighbors[i].g = g_temp;
						open_set.push_back(winner.neighbors[i]);
					}

					winner.neighbors[i].h = heuristic(winner.neighbors[i], end);
					winner.neighbors[i].f = winner.neighbors[i].h + winner.neighbors[i].g;
					winner.neighbors[i].parent = &winner;
				}
			}

		}
		
	}

}

a_star::~a_star()
{
}
