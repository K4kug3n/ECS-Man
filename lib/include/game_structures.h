#pragma once

#include <vector>
#include <string>

struct Position
{
	float x;
	float y;
};

struct Index
{
	int x;
	int y;
};

struct Size
{
	int width;
	int height;
};

using Speed = float;

struct Animation_infos
{
	int nb_animation = 0;
	int start_step = 0;
	int speed_step = 0;
};

struct Mob_infos
{
	Position position;
	Size size;
	Speed speed;
	Animation_infos animation;
};

struct Map_infos
{
	int nb_cols;
	int nb_rows;
	Size tile_size;
	std::vector<bool> collider_map;
	std::vector<size_t> id_map;
	std::string tileset_path;
};

struct Textures_infos
{
	std::string sprite_player_path;
	std::vector<std::string> sprite_ennemies_paths;
	std::string text_point_path;
};

struct Points_infos
{
	std::vector<Position> points_positions;
	Size point_size;
};

Position get_center(Position const& pos, Size const& Size);

//inline bool operator==(Index const& struct_1, Index const& struct_2)
//{
//	return (struct_1.x == struct_2.x && struct_1.y == struct_2.y);
//}
