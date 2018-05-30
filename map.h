#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

class map
{
public:
	map(std::string file_path ,std::string tileset_path);

	void load_tileset(std::string tileset_path);
	void load_map(std::string file_path);
	void create_map();
	void draw_map(sf::RenderWindow & render_window);

	bool check_collision(float x, float y, int w, int h);

	~map();

private: 

	void load_id_map(std::ifstream & file);
	void load_collider_map(std::ifstream & file);

	sf::Texture tileset;
	sf::VertexArray vertex_map;

	std::vector<size_t> id_map;
	std::vector<size_t> collider_map;
	size_t nb_y_tile;
	size_t nb_x_tile;
	size_t tile_size;
};