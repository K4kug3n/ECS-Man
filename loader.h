#pragma once

#include <vector>
#include <string>
#include <exception>

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
	map_infos get_map_info();
	std::vector<position> get_points_positions();

	~loader();

private:
	void extract_map(map_infos & infos, tinyxml2::XMLNode * map_node);

	tinyxml2::XMLDocument m_doc;
};
