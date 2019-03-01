#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

#include "map.h"
#include "a_star.h"
#include "loader.h"
#include "game_structures.h"

namespace ecs
{
	using id = size_t;
	using entities = std::vector<id>;

	struct physic
	{
		Position position_data;
		Size size_data;
	};
	struct physic_component
	{
		physic physic_data;
		id id_data;
	};
	using physics = std::vector<physic_component>;

	using celerity = Position;
	struct celerity_component
	{
		celerity celerity_data;
		id id_data;
	};
	using celerities = std::vector<celerity_component>;

	using Speed = float;
	struct speed_component
	{
		Speed speed_data;
		id id_data;
	};
	using speeds = std::vector<speed_component>;

	using health = int;
	struct health_component
	{
		health health_data;
		id id_data;
	};
	using healths = std::vector<health_component>;

	enum class type { mob, point };
	struct type_component
	{
		type type_data;
		id id_data;
	};
	using types = std::vector<type_component>;


	using sprite = sf::Sprite;
	struct sprite_component
	{
		sprite sprite_data;
		id id_data;
	};
	using sprites = std::vector<sprite_component>;

	enum class Direction { right, bottom, left, top };
	struct animation
	{
		Direction dir;
		int step;
		int max_step;
		int interval_time;
		size_t time_spended = 0;
	};
	struct animation_component
	{
		animation animation_data;
		id id_data;
	};
	using animations = std::vector<animation_component>;

	enum class Behavior { aggressive };
	struct ai
	{
		Behavior behavior;
	};
	struct ai_component
	{
		ai ai_data;
		id id_data;
	};
	using ais = std::vector<ai_component>;

	struct stage
	{
		map _map;
		entities _entities;
		physics _physics;
		celerities _celerities;
		speeds _speeds;
		healths _healths;
		types _types;
		sprites _sprites;
		animations _animations;
		ais _ais;
	};

	bool check_collision(Position const& b1_p, Size const& b1_s, Position const& b2_p, Size const& b2_s)
	{
		if (b1_p.x < b2_p.x + b2_s.width &&
			b1_p.x + b1_s.width > b2_p.x &&
			b1_p.y < b2_p.y + b2_s.height &&
			b1_s.height + b1_p.y > b2_p.y)
		{
			return true;
		}
		else { return false; }
	}

	int dir_to_int(Direction const& dir)
	{
		return static_cast<int>(dir);
	}

	template <typename Collection>
	typename Collection::value_type & get_component(Collection & collection, id const& id)
	{
		auto it = std::find_if(begin(collection), end(collection),
			[id](auto p) {return(p.id_data == id); });

		assert(it != end(collection));

		return (*it);
	}

	id create_entity(stage const& stage)
	{
		return stage._entities.empty() ? 1 : stage._entities.size() + 1;;
	}

	id add_mob(stage & stage, physic const& physic, Speed const& spd, sf::Texture const& texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._physics.push_back(physic_component{ physic, id });
		stage._celerities.push_back(celerity_component{ celerity{ 0, 0 }, id });
		stage._speeds.push_back(speed_component{ spd, id });
		stage._healths.push_back(health_component{ 3, id });
		stage._types.push_back(type_component{ type::mob, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	id add_point(stage & stage, physic const& physic, sf::Texture const& texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._physics.push_back(physic_component{ physic, id });
		stage._types.push_back(type_component{ type::point, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	void add_animation(stage & stage, id const& target, animation const& anim)
	{
		stage._animations.push_back(animation_component{ anim, target });
	}

	void add_ai(stage & stage, id const& target, Behavior const& behavior)
	{
		stage._ais.push_back(ai_component{ ai{ behavior }, target });
	}

	void remove_entity(stage & stage, id const& id)
	{
		const auto entities_it{ std::remove(stage._entities.begin(), stage._entities.end(), id) };
		stage._entities.erase(entities_it, stage._entities.end());

		const auto physics_it{ std::remove_if(stage._physics.begin(), stage._physics.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._physics.erase(physics_it, stage._physics.end());

		const auto celerities_it{ std::remove_if(stage._celerities.begin(), stage._celerities.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._celerities.erase(celerities_it, stage._celerities.end());

		const auto speeds_it{ std::remove_if(stage._speeds.begin(), stage._speeds.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._speeds.erase(speeds_it, stage._speeds.end());

		const auto healths_it{ std::remove_if(stage._healths.begin(), stage._healths.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._healths.erase(healths_it, stage._healths.end());

		const auto types_it{ std::remove_if(stage._types.begin(), stage._types.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._types.erase(types_it, stage._types.end());

		const auto sprites_it{ std::remove_if(stage._sprites.begin(), stage._sprites.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._sprites.erase(sprites_it, stage._sprites.end());

		const auto animations_it{ std::remove_if(stage._animations.begin(), stage._animations.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._animations.erase(animations_it, stage._animations.end());

		const auto ai_it{ std::remove_if(stage._ais.begin(), stage._ais.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._ais.erase(ai_it, stage._ais.end());
	}

	void remove_entities(stage & stage, std::vector<id> & entity_to_remove)
	{
		for (auto & entity : entity_to_remove)
		{
			remove_entity(stage, entity);
		}

		entity_to_remove.clear();
	}

	void set_celerity(stage & stage, id const& id, celerity const& new_celerity)
	{
		auto & celerity_component{ get_component(stage._celerities, id) };

		celerity_component.celerity_data = new_celerity;
	}

	void set_direction(stage & stage, id const& target, Direction const& dir)
	{
		auto & animation_component{ get_component(stage._animations, target) };

		animation_component.animation_data.dir = dir;
	}

	void get_damage(stage & level, id const& target, health damages_token)
	{
		auto & health_target{ get_component(level._healths, target).health_data };
		health_target -= damages_token;
		//CHECK IF DEAD
	}

	void entities_interaction(stage & level, id const& entity_1, id const& entity_2, std::vector<id> & entity_to_remove)
	{
		auto entity_1_t{ get_component(level._types, entity_1) };
		auto entity_2_t{ get_component(level._types, entity_2) };

		if (entity_1_t.type_data == type::mob && entity_2_t.type_data == type::point)
		{
			entity_to_remove.push_back(entity_2);
		}
		else if (entity_1_t.type_data == type::mob && entity_2_t.type_data == type::mob)
		{
			get_damage(level, entity_1, 1);
			get_damage(level, entity_2, 1);
		}
	}

	std::vector<Position> choose_path(a_star & path_finding, Position const& pos_target, Size const& size_target, Position const& final_pos)
	{
		std::vector<Position> path_1{ path_finding.create_center_path(pos_target.x, pos_target.y, final_pos.x, final_pos.y) };
		std::vector<Position> path_2{ path_finding.create_center_path(pos_target.x + size_target.width, pos_target.y + size_target.height, final_pos.x, final_pos.y) };

		if (path_1.size() > path_2.size())
		{
			return path_1;
		}
		else
		{
			return path_2;
		}

	}

	void update_positions(stage & stage)
	{
		for (auto & celerity : stage._celerities)
		{
			auto & physic_component{ get_component(stage._physics, celerity.id_data) };

			float next_x_position{ physic_component.physic_data.position_data.x + celerity.celerity_data.x };
			float next_y_position{ physic_component.physic_data.position_data.y + celerity.celerity_data.y };

			if (!stage._map.check_collision(next_x_position, next_y_position, physic_component.physic_data.size_data.width, physic_component.physic_data.size_data.height))
			{
				physic_component.physic_data.position_data.x = next_x_position;
				physic_component.physic_data.position_data.y = next_y_position;
			}

			celerity.celerity_data.x = 0;
			celerity.celerity_data.y = 0;
		}
	}

	void update_collisions(stage & stage, id const& target)
	{
		auto target_physic{ get_component(stage._physics, target) };

		std::vector<id> entity_to_remove;

		for (auto & entity_p : stage._physics)
		{
			if (entity_p.id_data != target)
			{

				if (check_collision(target_physic.physic_data.position_data, target_physic.physic_data.size_data,
					entity_p.physic_data.position_data, entity_p.physic_data.size_data))
				{
					entities_interaction(stage, target, entity_p.id_data, entity_to_remove);
				}
			}
		}

		remove_entities(stage, entity_to_remove);

	}

	void update_ai(stage & stage, ai_component const& target, a_star & path_finding, id const& player)
	{
		if (target.ai_data.behavior == Behavior::aggressive)
		{
			physic_component target_physic{ get_component(stage._physics, target.id_data) };

			Position target_center{ get_center(target_physic.physic_data.position_data, target_physic.physic_data.size_data) };
			Position player_position{ get_component(stage._physics, player).physic_data.position_data };

			std::vector<Position> pos_path{ choose_path(path_finding, target_physic.physic_data.position_data, target_physic.physic_data.size_data, player_position) };
			pos_path.pop_back();

			if (!pos_path.empty())
			{
				Position next_position{ pos_path.back() };

				Speed spd{ get_component(stage._speeds, target.id_data).speed_data };

				celerity acceleration{ 0, 0 };

				if (next_position.x - spd >= target_center.x)
				{
					acceleration.x += spd;
				}
				else if (next_position.x + spd <= target_center.x)
				{
					acceleration.x -= spd;
				}

				if (next_position.y - spd >= target_center.y)
				{
					acceleration.y += spd;
				}
				else if (next_position.y + spd <= target_center.y)
				{
					acceleration.y -= spd;
				}

				ecs::set_celerity(stage, target.id_data, acceleration);

			}

		}
	}

	void update_ais(stage & stage, a_star & path_finding, id const& player)
	{
		for (auto & entity : stage._ais)
		{
			update_ai(stage, entity, path_finding, player);
		}
	}

	void update_animations(stage & stage)
	{
		for (auto & animation_component : stage._animations)
		{
			const auto & size_component{ get_component(stage._physics, animation_component.id_data).physic_data.size_data };
			auto & sprite_component{ get_component(stage._sprites, animation_component.id_data) };

			sprite_component.sprite_data.setTextureRect(sf::IntRect{
				animation_component.animation_data.step * size_component.width,
				dir_to_int(animation_component.animation_data.dir) * size_component.height,
				size_component.width,
				size_component.height
				});
		}
	}

	void update_animations_step(stage & stage)
	{
		for (auto & animation_component : stage._animations)
		{
			animation_component.animation_data.time_spended++;

			if (animation_component.animation_data.time_spended == animation_component.animation_data.interval_time)
			{
				animation_component.animation_data.step++;
				if (animation_component.animation_data.step > animation_component.animation_data.max_step) 
				{
					animation_component.animation_data.step = 0; 
				}

				animation_component.animation_data.time_spended = 0;
			}

		}
	}

	void update_sprites_position(stage & stage)
	{
		for (auto & sprite : stage._sprites)
		{
			auto & physic_component{ get_component(stage._physics, sprite.id_data) };

			sprite.sprite_data.setPosition(physic_component.physic_data.position_data.x, physic_component.physic_data.position_data.y);
		}
	}

	void update_view(sf::RenderWindow & window, stage & stage, id const& player)
	{
		auto player_physic{ get_component(stage._physics, player) };

		float screen_width{ static_cast<float>(window.getSize().x) };
		float screen_height{ static_cast<float>(window.getSize().y) };

		Map_infos infos_map_loaded{ stage._map.get_loaded_infos() };

		float center_x{ player_physic.physic_data.position_data.x + player_physic.physic_data.size_data.width / 2 - screen_width / 2 };
		if (center_x < 0)
		{
			center_x = 0;
		}
		else if (center_x + screen_width > infos_map_loaded.nb_cols * infos_map_loaded.tile_size.width)
		{
			center_x = infos_map_loaded.nb_cols * infos_map_loaded.tile_size.width - screen_width;
		}
		float center_y{ player_physic.physic_data.position_data.y + player_physic.physic_data.size_data.height / 2 - screen_height / 2 };
		if (center_y < 0)
		{
			center_y = 0;
		}
		else if (center_y + screen_height > infos_map_loaded.nb_rows * infos_map_loaded.tile_size.height)
		{
			center_y = infos_map_loaded.nb_rows * infos_map_loaded.tile_size.height - screen_height;
		}

		window.setView(sf::View{ sf::FloatRect{ center_x, center_y,  screen_width, screen_height } });
	}

	void display_entities(stage & stage, sf::RenderWindow & window)
	{
		for (auto & entity : stage._sprites)
		{
			window.draw(entity.sprite_data);
		}
	}
}

sf::Texture load_texture(std::string file_path)
{
	sf::Texture texture;
	texture.loadFromFile(file_path);
	return texture;
}

struct texture_pack
{
	sf::Texture _player;
	std::vector<sf::Texture> _ennemies;
	sf::Texture _point;
};

texture_pack create_texture_pack(Textures_infos const& infos)
{
	texture_pack textures;

	textures._player = load_texture(infos.sprite_player_path);
	textures._point = load_texture(infos.text_point_path);

	for (auto const& path : infos.sprite_ennemies_paths)
	{
		textures._ennemies.push_back(load_texture(path));
	}

	return textures;
}

void keyboard_input(ecs::stage & stage, ecs::id target)
{
	ecs::Speed acceleration{ ecs::get_component(stage._speeds, target).speed_data };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		ecs::set_celerity(stage, target, ecs::celerity{ 0, -acceleration });
		ecs::set_direction(stage, target, ecs::Direction::top);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		ecs::set_celerity(stage, target, ecs::celerity{ 0, acceleration });
		ecs::set_direction(stage, target, ecs::Direction::bottom);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		ecs::set_celerity(stage, target, ecs::celerity{ -acceleration, 0 });
		ecs::set_direction(stage, target, ecs::Direction::left);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		ecs::set_celerity(stage, target, ecs::celerity{ acceleration, 0 });
		ecs::set_direction(stage, target, ecs::Direction::right);
	}

}

ecs::id add_player(Mob_infos const& infos, texture_pack const& textures, ecs::stage & level)
{
	auto id{ ecs::add_mob(level, ecs::physic{ infos.position, infos.size }, infos.speed, textures._player) };
	
	if (infos.animation.nb_animation != 0)
	{
		ecs::add_animation(level, id, ecs::animation{ecs::Direction::right,
													infos.animation.start_step,
													infos.animation.nb_animation,
													infos.animation.speed_step });
	}

	return id;
}

void add_ennemie(Mob_infos const& infos, sf::Texture const& texture, ecs::stage & level)
{
	auto id{ ecs::add_mob(level, ecs::physic{ infos.position, infos.size }, infos.speed, texture) };
	ecs::add_ai(level, id, ecs::Behavior::aggressive);

	if (infos.animation.nb_animation != 0)
	{
		ecs::add_animation(level, id, ecs::animation{ ecs::Direction::right,
			infos.animation.start_step,
			infos.animation.nb_animation,
			infos.animation.speed_step });
	}

}

void add_ennemies(std::vector<Mob_infos> const& infos, texture_pack const& textures, ecs::stage & level)
{
	for (size_t i{ 0 }; i < infos.size(); i++)
	{
		add_ennemie(infos[i], textures._ennemies[i], level);
	}
}

void add_points(Points_infos const& infos, texture_pack const& textures, ecs::stage & level)
{
	for (auto const& Position : infos.points_positions)
	{
		ecs::add_point(level, ecs::physic{ Position, infos.point_size }, textures._point);
	}
}

int main()
{
	loader loader{};
	loader.load("level_1.xml");

	Map_infos map_infos;
	try
	{
		map_infos = loader.get_map_infos();
	}
	catch (LoaderException & e)
	{
		std::cout << e.what() << std::endl;
		return -1;
	}

	ecs::stage level_1{ map{ map_infos }};
	a_star a_star{ map_infos };

	texture_pack textures{ create_texture_pack( loader.get_textures_infos() ) };

	auto player = add_player(loader.get_player_infos(), textures, level_1);
	add_ennemies(loader.get_ennemies_infos(), textures, level_1);
	add_points( loader.get_points_infos(), textures, level_1 );

	sf::RenderWindow window(sf::VideoMode{900 , 675, 32 }, "PacMan");
	window.setFramerateLimit(60);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}
 
		keyboard_input(level_1, player);

		ecs::update_ais(level_1, a_star, player);
		ecs::update_positions(level_1);
		ecs::update_collisions(level_1, player);

		ecs::update_animations_step(level_1);
		ecs::update_animations(level_1);

		ecs::update_sprites_position(level_1);
		ecs::update_view(window, level_1, player);

		window.clear();

		level_1._map.draw_map(window);
		ecs::display_entities(level_1, window);

		window.display();
	}

	return 0;
}

//TODO
//Add collision between 2 entity
//Modifie pathfinding
//Check if dead