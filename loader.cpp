#include "loader.h"

bool xml_successfull(tinyxml2::XMLError const& error)
{
	return (error == tinyxml2::XMLError::XML_SUCCESS);
}

std::vector<bool> extract_collider_map(tinyxml2::XMLNode * collider_node)
{
	tinyxml2::XMLElement * collider_element{ collider_node->FirstChildElement("C") };
	if (collider_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'C' element" };
	}

	std::vector<bool> collider_map{};
	while (collider_element != nullptr)
	{
		bool value;
		if (!xml_successfull(collider_element->QueryBoolAttribute("v", &value)))
		{
			throw LoaderException{ "Search of bool in 'C' failed" };
		}
		collider_map.push_back(value);

		collider_element = collider_element->NextSiblingElement("C");
	}

	return collider_map;
}

std::vector<size_t> extract_id_map(tinyxml2::XMLNode * id_node)
{
	tinyxml2::XMLElement * id_element{ id_node->FirstChildElement("G") };
	if (id_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'G' element" };
	}

	std::vector<size_t> id_map{};
	while (id_element != nullptr)
	{
		size_t value;
		if (!xml_successfull(id_element->QueryUnsignedAttribute("v", &value)))
		{
			throw LoaderException{ "Search of bool in 'G' failed" };
		}
		id_map.push_back(value);

		id_element = id_element->NextSiblingElement("G");
	}

	return id_map;
}

loader::loader()
{
}

bool loader::load(std::string const& file_path)
{
	return xml_successfull( m_doc.LoadFile(file_path.c_str()) );
}

void loader::extract_map(map_infos & infos, tinyxml2::XMLNode * map_node)
{
	tinyxml2::XMLNode *collider_node{ map_node->FirstChildElement("Collider") };
	if (collider_node == nullptr)
	{
		throw LoaderException{ "Cannot get 'Collider' element" };
	}
	infos.collider_map = extract_collider_map(collider_node);

	tinyxml2::XMLNode *id_node{ map_node->FirstChildElement("Graphic") };
	if (id_node == nullptr)
	{
		throw LoaderException{ "Cannot get 'Graphic' element" };
	}
	infos.id_map = extract_id_map(id_node);
}

map_infos loader::get_map_info()
{
	map_infos infos;

	tinyxml2::XMLNode *tile_node{ m_doc.FirstChildElement("Tile") };
	if (tile_node == nullptr)
	{
		throw LoaderException{ "Cannot get 'Tile' element" };
	}

	tinyxml2::XMLElement *tile_element{ tile_node->FirstChildElement("Size") };
	if (tile_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'Size' element" };
	}

	if (!xml_successfull( tile_element->QueryIntText(&infos.tile_size) ))
	{
		throw LoaderException{ "Search of int in 'Size' failed" };
	}

	tinyxml2::XMLNode *map_node{ m_doc.FirstChildElement("Map") };
	if (map_node == nullptr)
	{
		throw LoaderException{ "Cannot get 'Map' element" };
	}

	tinyxml2::XMLElement *cols_element{ map_node->FirstChildElement("Cols") };
	if (cols_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'Cols' element" };
	}
	
	if (!xml_successfull( cols_element->QueryIntText(&infos.nb_x_tile) ))
	{
		throw LoaderException{ "Search of int in 'Cols' failed" };
	}

	tinyxml2::XMLElement *height_element{ map_node->FirstChildElement("Rows") };
	if (height_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'Rows' element" };
	}

	if(!xml_successfull( height_element->QueryIntText(&infos.nb_y_tile) ))
	{
		throw LoaderException{ "Search of int in 'Rows' failed" };
	}

	extract_map(infos, map_node);

	return infos;
}

std::vector<position> loader::get_points_positions()
{
	tinyxml2::XMLNode *points_node{ m_doc.FirstChildElement("Points") };
	if (points_node == nullptr)
	{
		throw LoaderException{ "Cannot get 'Points' element" };
	}
	
	tinyxml2::XMLElement *point_element{ points_node->FirstChildElement("Point") };
	if (point_element == nullptr)
	{
		throw LoaderException{ "Cannot get 'Point' element" };
	}

	std::vector<position> points_positions{};
	while (point_element != nullptr)
	{
		float x;
		if (!xml_successfull( point_element->QueryFloatAttribute("x", &x)) )
		{
			throw LoaderException{ "Search of float in 'Point' failed" };
		}

		float y;
		if (!xml_successfull(point_element->QueryFloatAttribute("y", &y)))
		{
			throw LoaderException{ "Search of float in 'Point' failed" };
		}

		points_positions.push_back(position{ x, y });
		point_element = point_element->NextSiblingElement("Point");
	}

	return points_positions;
}

loader::~loader()
{
}
