#include "map.h"

#include <fstream>
#include <iostream>
#include <cmath>


map::map(std::string file_path, std::string tileset_path)
{
	load_map(file_path);
	load_tileset(tileset_path);

	create_map();
}

void map::load_tileset(std::string tileset_path)
{
	tileset.loadFromFile(tileset_path);
}

void map::load_map(std::string file_path)
{
	std::ifstream file{ file_path };

	file >> nb_x_tile;
	file >> nb_y_tile;
	file >> tile_size;

	load_id_map(file);
	load_collider_map(file);

	file.close();
}

void map::load_id_map(std::ifstream & file)
{
	for (size_t i{ 0 }; i < nb_x_tile * nb_y_tile; i++) 
	{
		unsigned temp;
		file >> temp;
		id_map.push_back(temp);
	}
}

void map::load_collider_map(std::ifstream & file)
{
	for (size_t i{ 0 }; i < nb_x_tile * nb_y_tile; i++)
	{
		bool temp;
		file >> temp;
		collider_map.push_back(temp);
	}
}

void map::create_map()
{
	vertex_map.setPrimitiveType(sf::Quads);
	vertex_map.resize(nb_x_tile * nb_y_tile * 4);

	for (size_t y(0); y < nb_y_tile; y++) {
		for (size_t x(0); x < nb_x_tile; x++) {

			size_t actualTile = id_map[nb_x_tile * y + x] - 1;

			sf::Vertex *quad = &vertex_map[(y * nb_x_tile + x) * 4];

			int temp = tileset.getSize().x / tile_size;

			int tv = actualTile / temp;
			int tu = actualTile % temp;

			quad[0].position = sf::Vector2f(x * tile_size, y * tile_size);
			quad[1].position = sf::Vector2f((x + 1) * tile_size, y * tile_size);
			quad[2].position = sf::Vector2f((x + 1) * tile_size, (y + 1) * tile_size);
			quad[3].position = sf::Vector2f(x * tile_size, (y + 1) * tile_size);

			quad[0].texCoords = sf::Vector2f(tu * tile_size, tv * tile_size);
			quad[1].texCoords = sf::Vector2f((tu + 1) * tile_size, tv * tile_size);
			quad[2].texCoords = sf::Vector2f((tu + 1) * tile_size, (tv + 1) * tile_size);
			quad[3].texCoords = sf::Vector2f(tu * tile_size, (tv + 1) * tile_size);
		}
	}
}

void map::draw_map(sf::RenderWindow & render_window)
{
	render_window.draw(vertex_map, sf::RenderStates{ &tileset });
}

bool map::check_collision(float x, float y, int w, int h)
{
	float x1_map{ std::floor (x / tile_size) };
	float y1_map{ std::floor (y / tile_size) };
	float x2_map{ std::floor((x + w) / tile_size) };
	float y2_map{ std::floor((y + h) / tile_size) };

	float tile1{ y1_map * nb_x_tile + x1_map };
	float tile2{ y1_map * nb_x_tile + x2_map };
	float tile3{ y2_map * nb_x_tile + x1_map };
	float tile4{ y2_map * nb_x_tile + x2_map };

	//std::cout << "tile 1 : " << tile1 << std::endl;
	//std::cout << "tile 2 : " << tile2 << std::endl;

	if (collider_map[tile1]) { return true; } 
	if (collider_map[tile2]) { return true; }
	if (collider_map[tile3]) { return true; }
	if (collider_map[tile4]) { return true; }

	return false;
}


map::~map()
{
}


