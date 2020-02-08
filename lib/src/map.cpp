#include "map.h"

#include <fstream>
#include <iostream>
#include <cmath>


Map::Map(Map_infos const& infos) :
	m_infos{ infos },
	m_tileset{load_tileset(infos.tileset_path)}
{
	create_map();
}

sf::Texture Map::load_tileset(std::string const& tileset_path)
{
	sf::Texture result;
	result.loadFromFile(tileset_path);

	return result;
}

void Map::create_map()
{
	m_vertex_map.setPrimitiveType(sf::Quads);
	m_vertex_map.resize(m_infos.nb_cols * m_infos.nb_rows * 4);

	for (int y(0); y < m_infos.nb_rows; y++) {
		for (int x(0); x < m_infos.nb_cols; x++) {

			size_t actualTile = m_infos.id_map[m_infos.nb_cols * y + x] - 1;

			sf::Vertex *quad = &m_vertex_map[(y * m_infos.nb_cols + x) * 4];

			int temp = m_tileset.getSize().x / m_infos.tile_size.width;

			int tv = actualTile / temp;
			int tu = actualTile % temp;

			quad[0].position = sf::Vector2f(x *  m_infos.tile_size.width, y *  m_infos.tile_size.height);
			quad[1].position = sf::Vector2f((x + 1) *  m_infos.tile_size.width, y *  m_infos.tile_size.height);
			quad[2].position = sf::Vector2f((x + 1) *  m_infos.tile_size.width, (y + 1) *  m_infos.tile_size.height);
			quad[3].position = sf::Vector2f(x *  m_infos.tile_size.width, (y + 1) *  m_infos.tile_size.height);

			quad[0].texCoords = sf::Vector2f(tu *  m_infos.tile_size.width, tv * m_infos.tile_size.height);
			quad[1].texCoords = sf::Vector2f((tu + 1) *  m_infos.tile_size.width, tv *  m_infos.tile_size.height);
			quad[2].texCoords = sf::Vector2f((tu + 1) *  m_infos.tile_size.width, (tv + 1) *  m_infos.tile_size.height);
			quad[3].texCoords = sf::Vector2f(tu *  m_infos.tile_size.width, (tv + 1) *  m_infos.tile_size.height);
		}
	}
}

void Map::draw_map(sf::RenderWindow & render_window)
{
	render_window.draw(m_vertex_map, sf::RenderStates{ &m_tileset });
}

bool Map::check_collision(float x, float y, int w, int h)
{
	float x1_map{ std::floor (x / m_infos.tile_size.width) };
	float y1_map{ std::floor (y / m_infos.tile_size.height) };
	float x2_map{ std::floor((x + w) / m_infos.tile_size.width) };
	float y2_map{ std::floor((y + h) / m_infos.tile_size.height) };

	float tile1{ y1_map * m_infos.nb_cols + x1_map };
	float tile2{ y1_map * m_infos.nb_cols + x2_map };
	float tile3{ y2_map * m_infos.nb_cols + x1_map };
	float tile4{ y2_map * m_infos.nb_cols + x2_map };

	if (m_infos.collider_map[tile1]) { return true; }
	if (m_infos.collider_map[tile2]) { return true; }
	if (m_infos.collider_map[tile3]) { return true; }
	if (m_infos.collider_map[tile4]) { return true; }

	return false;
}

Map_infos Map::get_loaded_infos() const
{
	return m_infos;
}


Map::~Map()
{
}
