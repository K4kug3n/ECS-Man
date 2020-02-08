#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "game_structures.h"

class Map
{
public:
	Map(Map_infos const& infos);

	void create_map();
	void draw_map(sf::RenderWindow & render_window);

	bool check_collision(float x, float y, int w, int h);
	Map_infos get_loaded_infos() const;

	~Map();

private:
	sf::Texture load_tileset(std::string const& tileset_path);

	sf::Texture m_tileset;
	sf::VertexArray m_vertex_map;

	Map_infos m_infos;
};
