#pragma once

#include <vector>
#include <string>
#include <stdexcept>

#include "tinyxml2.h"
#include "game_structures.h"


class LoaderException : public std::runtime_error
{
public:
	LoaderException(std::string const& error) :
		std::runtime_error{error}
	{}

	virtual const char* what() const throw()
	{
		return runtime_error::what();
	}
};

class loader
{
public:
	loader();

	bool load(std::string const& file_path);
	Map_infos get_map_infos();
	Mob_infos get_player_infos();
	Textures_infos get_textures_infos();
	std::vector<Mob_infos> get_ennemies_infos();
	Points_infos get_points_infos();

	~loader();

private:
	void extract_map(Map_infos & infos, tinyxml2::XMLNode * map_node);
	Mob_infos extract_ennemie_infos(tinyxml2::XMLElement * ennemie_element);
	Animation_infos extract_animation_infos(tinyxml2::XMLElement * animation_element);

	tinyxml2::XMLDocument m_doc;
};
