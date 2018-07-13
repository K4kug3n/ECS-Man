#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "game_structures.h"

class map
{
public:
	map(map_infos const& infos, std::string const& tileset_path);

	void load_tileset(std::string const& tileset_path);
	void load_map_infos(map_infos const& infos);
	void create_map();
	void draw_map(sf::RenderWindow & render_window);

	bool check_collision(float x, float y, int w, int h);

	~map();

private: 
	sf::Texture tileset;
	sf::VertexArray vertex_map;

	std::vector<size_t> id_map;
	std::vector<bool> collider_map;
	int nb_y_tile;
	int nb_x_tile;
	int tile_size;
};