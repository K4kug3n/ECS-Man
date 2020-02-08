#include "loader.h"

bool xml_successfull(tinyxml2::XMLError const& error)
{
	return (error == tinyxml2::XMLError::XML_SUCCESS);
}

tinyxml2::XMLElement * first_child_element(tinyxml2::XMLNode * parent_node, std::string const& tag_name)
{
	tinyxml2::XMLElement * child_element{ parent_node->FirstChildElement(tag_name.c_str()) };
	if (!child_element)
	{
		throw LoaderException{ "Cannot get " + tag_name + " element" };
	}

	return child_element;
}

tinyxml2::XMLNode * get_node(tinyxml2::XMLDocument & doc, std::string const& tag_name)
{
	tinyxml2::XMLNode *node{ doc.FirstChildElement( tag_name.c_str() ) };
	if (!node)
	{
		throw LoaderException{ "Cannot get '" + tag_name + "' element" };
	}

	return node;
}

std::vector<bool> extract_collider_map(tinyxml2::XMLNode * collider_node)
{
	tinyxml2::XMLElement * collider_element{ first_child_element(collider_node, "C") };

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
	tinyxml2::XMLElement * id_element{ first_child_element(id_node, "G") };

	std::vector<size_t> id_map{};
	while (id_element != nullptr)
	{
		unsigned int value;
		if (!xml_successfull(id_element->QueryUnsignedAttribute("v", &value)))
		{
			throw LoaderException{ "Search of bool in 'G' failed" };
		}
		id_map.push_back(value);

		id_element = id_element->NextSiblingElement("G");
	}

	return id_map;
}

Position extract_position(tinyxml2::XMLElement * parent_element)
{
	Position result;

	if (!xml_successfull(parent_element->QueryFloatAttribute("x", &result.x) ))
	{
		throw LoaderException{ "Search of float in 'Position' failed" };
	}

	if (!xml_successfull(parent_element->QueryFloatAttribute("y", &result.y) ))
	{
		throw LoaderException{ "Search of float in 'Position' failed" };
	}

	return result;
}

Size extract_size(tinyxml2::XMLElement * parent_element)
{
	Size result;

	if (!xml_successfull(parent_element->QueryIntAttribute("width", &result.width)))
	{
		throw LoaderException{ "Search of int in 'Size' failed" };
	}

	if (!xml_successfull(parent_element->QueryIntAttribute("height", &result.height)))
	{
		throw LoaderException{ "Search of int in 'Size' failed" };
	}

	return result;
}

Speed extract_speed(tinyxml2::XMLElement * parent_element)
{
	Speed result;

	if (!xml_successfull( parent_element->QueryFloatText(&result) ))
	{
		throw LoaderException{ "Search of float in 'Speed' failed" };
	}

	return result;
}

std::string extract_path(tinyxml2::XMLElement * parent_element)
{	
	const char * result;

	if (!xml_successfull(parent_element->QueryStringAttribute("path", &result) ))
	{
		throw LoaderException{ "Search of string in 'path' failed" };
	}

	return result;
}


Loader::Loader()
{
}

bool Loader::load(std::string const& file_path)
{
	return xml_successfull( m_doc.LoadFile(file_path.c_str()) );
}

void Loader::extract_map(Map_infos & infos, tinyxml2::XMLNode * map_node)
{
	tinyxml2::XMLNode *collider_node{ first_child_element(map_node, "Collider") };

	infos.collider_map = extract_collider_map(collider_node);

	tinyxml2::XMLNode *id_node{ first_child_element(map_node, "Graphic") };

	infos.id_map = extract_id_map(id_node);
}


Map_infos Loader::get_map_infos()
{
	Map_infos infos;

	tinyxml2::XMLNode *map_node{ get_node(m_doc, "Map") };

	tinyxml2::XMLElement *size_element{ first_child_element(map_node, "Size") };
	infos.tile_size = extract_size(size_element);

	tinyxml2::XMLElement *cols_element{ first_child_element(map_node, "Cols") };

	if (!xml_successfull( cols_element->QueryIntText(&infos.nb_cols) ))
	{
		throw LoaderException{ "Search of int in 'Cols' failed" };
	}

	tinyxml2::XMLElement *height_element{ first_child_element(map_node, "Rows") };

	if(!xml_successfull( height_element->QueryIntText(&infos.nb_rows) ))
	{
		throw LoaderException{ "Search of int in 'Rows' failed" };
	}

	extract_map(infos, map_node);

	tinyxml2::XMLElement *tileset_element{ first_child_element(map_node, "Tileset") };
	infos.tileset_path = extract_path(tileset_element);

	return infos;
}

Mob_infos Loader::get_player_infos()
{
	tinyxml2::XMLNode *player_node{ get_node(m_doc, "Player") };

	tinyxml2::XMLElement *position_element{ first_child_element(player_node, "Position") };
	tinyxml2::XMLElement *size_element{ first_child_element(player_node, "Size") };
	tinyxml2::XMLElement *speed_element{ first_child_element(player_node, "Speed") };

	Animation_infos anim_infos;
	tinyxml2::XMLElement *animation_element{ player_node->FirstChildElement("Animation") };
	if (animation_element)
	{
		anim_infos = extract_animation_infos(animation_element);
	}

	return Mob_infos{extract_position(position_element),
						extract_size(size_element),
						extract_speed(speed_element),
						anim_infos };
}

Textures_infos Loader::get_textures_infos()
{	
	tinyxml2::XMLNode *textures_node{ get_node(m_doc, "Textures") };
	
	Textures_infos infos;

	tinyxml2::XMLElement *player_element{ first_child_element(textures_node, "Sprite_Player") };
	infos.sprite_player_path = extract_path(player_element);

	tinyxml2::XMLElement *point_element{ first_child_element(textures_node, "Texture_Point") };
	infos.text_point_path = extract_path(point_element);

	tinyxml2::XMLNode *ennemies_element{ textures_node->FirstChildElement("Sprite_Ennemies") };
	tinyxml2::XMLElement *ennemie_element{ first_child_element(ennemies_element, "Sprite_Ennemie") };

	while (ennemie_element)
	{
		infos.sprite_ennemies_paths.push_back( extract_path(ennemie_element) );
		ennemie_element = ennemie_element->NextSiblingElement("Sprite_Ennemie");
	}

	return infos;
}

Mob_infos Loader::extract_ennemie_infos(tinyxml2::XMLElement * ennemie_element)
{
	tinyxml2::XMLElement *position_element{ first_child_element(ennemie_element, "Position") };
	tinyxml2::XMLElement *size_element{ first_child_element(ennemie_element, "Size") };
	tinyxml2::XMLElement *speed_element{ first_child_element(ennemie_element, "Speed") };

	Animation_infos anim_infos;
	tinyxml2::XMLElement *animation_element{ ennemie_element->FirstChildElement("Animation") };
	if (animation_element)
	{
		anim_infos = extract_animation_infos(animation_element);
	}

	return Mob_infos{ extract_position(position_element),
						extract_size(size_element),
						extract_speed(speed_element),
						anim_infos };
}

Animation_infos Loader::extract_animation_infos(tinyxml2::XMLElement * animation_element)
{
	Animation_infos infos;

	if (!xml_successfull(animation_element->QueryIntAttribute("nb", &infos.nb_animation)))
	{
		throw LoaderException{ "Search of int in 'Animation' failed" };
	}
	infos.nb_animation -= 1; //We start to count at 0

	if (!xml_successfull(animation_element->QueryIntAttribute("actual_step", &infos.start_step)))
	{
		throw LoaderException{ "Search of int in 'Animation' failed" };
	}
	infos.start_step -= 1; //We start to count at 0

	if (!xml_successfull(animation_element->QueryIntAttribute("time", &infos.speed_step)))
	{
		throw LoaderException{ "Search of int in 'Animation' failed" };
	}

	return infos;
}

std::vector<Mob_infos> Loader::get_ennemies_infos()
{
	tinyxml2::XMLNode * ennemies_node( get_node(m_doc, "Ennemies") );

	std::vector<Mob_infos> infos;
	tinyxml2::XMLElement * ennemie_element( first_child_element(ennemies_node, "Ennemie") );

	while (ennemie_element)
	{
		infos.push_back( extract_ennemie_infos(ennemie_element) );
		ennemie_element = ennemie_element->NextSiblingElement("Ennemie");
	}

	return infos;
}

Points_infos Loader::get_points_infos()
{
	tinyxml2::XMLNode *points_node{ get_node(m_doc, "Points") };

	tinyxml2::XMLElement *point_element{ first_child_element(points_node, "Point") };

	Points_infos infos;
	while (point_element)
	{
		infos.points_positions.push_back( extract_position(point_element) );

		point_element = point_element->NextSiblingElement("Point");
	}

	tinyxml2::XMLElement *size_element{ first_child_element(points_node, "Size") };
	infos.point_size = extract_size(size_element);

	return infos;
}

Loader::~Loader()
{
}
