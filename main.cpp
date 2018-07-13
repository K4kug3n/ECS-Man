#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

#include "map.h"
#include "a_star.h"
#include "loader.h"

namespace ecs
{
	using id = size_t;
	using entities = std::vector<id>;

	struct position_component
	{
		position position_data;
		id id_data;
	};
	using positions = std::vector<position_component>;
	struct size
	{
		int width;
		int height;
	};
	struct size_component
	{
		size size_data;
		id id_data;
	};
	using sizes = std::vector<size_component>;


	using celerity = position;
	struct celerity_component
	{
		celerity celerity_data;
		id id_data;
	};
	using celerities = std::vector<celerity_component>;


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


	enum class Direction {right, bottom, left, top};
	struct animation
	{
		Direction dir;
		int step;
		size_t time_spended;
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
		positions _positions;
		sizes _sizes;
		celerities _celerities;
		types _types;
		sprites _sprites;
		animations _animations;
		ais _ais;
	};

	bool check_collision(position const& b1_p, size const& b1_s, position const& b2_p, size const& b2_s)
	{
		if (b1_p.x < b2_p.x + b2_s.width &&
			b1_p.x + b1_s.width > b2_p.x &&
   			b1_p.y < b2_p.y + b2_s.height &&
   			b1_s.height + b1_p.y > b2_p.y) 
		   { return true; }
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

	id add_mob(stage & stage, position const& pos, size const& s, sf::Texture const& texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._positions.push_back(position_component{ pos, id });
		stage._sizes.push_back(size_component{ s, id });
		stage._celerities.push_back(celerity_component{ celerity{0, 0}, id });
		stage._types.push_back(type_component{ type::mob, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	id add_point(stage & stage, position const& position, size const& s, sf::Texture const& texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._positions.push_back(position_component{ position, id });
		stage._sizes.push_back(size_component{ s, id });
		stage._types.push_back(type_component{ type::point, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	void add_animation(stage & stage, id const& target, Direction const& dir, int state)
	{
		stage._animations.push_back(animation_component{ animation{ dir, state, 0 }, target });
	}

	void add_ai(stage & stage, id const& target, Behavior const& behavior)
	{
		stage._ais.push_back(ai_component{ ai{ behavior }, target });
	}

	void remove_entity(stage & stage, id const& id)
	{
		const auto entities_it{ std::remove(stage._entities.begin(), stage._entities.end(), id) };
		stage._entities.erase(entities_it, stage._entities.end());

		const auto positions_it{ std::remove_if(stage._positions.begin(), stage._positions.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._positions.erase(positions_it, stage._positions.end());

		const auto sizes_it{ std::remove_if(stage._sizes.begin(), stage._sizes.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._sizes.erase(sizes_it, stage._sizes.end());

		const auto celerities_it{ std::remove_if(stage._celerities.begin(), stage._celerities.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._celerities.erase(celerities_it, stage._celerities.end());

		const auto types_it{ std::remove_if(stage._types.begin(), stage._types.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._types.erase(types_it, stage._types.end());

		const auto sprites_it{ std::remove_if(stage._sprites.begin(), stage._sprites.end(),
			[id](auto p) {return (p.id_data == id); }) };
		stage._sprites.erase(sprites_it, stage._sprites.end());

		const auto animations_it{std::remove_if(stage._animations.begin(), stage._animations.end(),
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

	void entities_interaction(stage & stage, id const& entity_1, id const& entity_2, std::vector<id> & entity_to_remove)
	{
		auto entity_1_t{ get_component(stage._types, entity_1) };
		auto entity_2_t{ get_component(stage._types, entity_2) };

		if (entity_1_t.type_data == type::mob && entity_2_t.type_data == type::point)
		{
			entity_to_remove.push_back(entity_2);
		}
	}

	void add_celerity(stage & stage, id const& id, celerity const& new_celerity)
	{
		auto & celerity_component{ get_component(stage._celerities, id) };

		celerity_component.celerity_data = new_celerity;
	}

	void set_direction(stage & stage, id const& target, Direction const& dir)
	{
		auto & animation_component{ get_component(stage._animations, target) };

		animation_component.animation_data.dir = dir;
	}

	void update_positions(stage & stage)
	{
		for (auto & celerity : stage._celerities)
		{
			auto & position_component{ get_component(stage._positions, celerity.id_data) };
			auto size_component{ get_component(stage._sizes, celerity.id_data) };

			float next_x_position{ position_component.position_data.x + celerity.celerity_data.x };
			float next_y_position{ position_component.position_data.y + celerity.celerity_data.y };

			if (!stage._map.check_collision(next_x_position, next_y_position, size_component.size_data.width, size_component.size_data.height))
			{
				position_component.position_data.x = next_x_position;
				position_component.position_data.y = next_y_position;
			}

			celerity.celerity_data.x = 0;
			celerity.celerity_data.y = 0;
		}
	}

	void update_collisions(stage & stage, id const& target)
	{
		auto target_p{ get_component(stage._positions, target) };
		auto target_s{ get_component(stage._sizes, target) };

		std::vector<id> entity_to_remove;

		for (auto & entity_p : stage._positions)
		{
			if (entity_p.id_data != target)
			{
				auto entity_s{ get_component(stage._sizes, entity_p.id_data) };

				if (check_collision(target_p.position_data, target_s.size_data,
					entity_p.position_data, entity_s.size_data))
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
			position target_position{ get_component(stage._positions, target.id_data).position_data };
			position player_position{ get_component(stage._positions, player).position_data };

			std::vector<position> pos_path{ path_finding.create_path(target_position.x, target_position.y, player_position.x, player_position.y) };

			//TODO
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
			const auto & size_component{ get_component(stage._sizes, animation_component.id_data) };
			auto & sprite_component{ get_component(stage._sprites, animation_component.id_data) };

			sprite_component.sprite_data.setTextureRect( sf::IntRect{
				animation_component.animation_data.step * size_component.size_data.width,
				dir_to_int(animation_component.animation_data.dir) * size_component.size_data.height,
				size_component.size_data.width,
				size_component.size_data.height
			});
		}
	}

	void update_animations_step(stage & stage)
	{
		for (auto & animation_component : stage._animations)
		{
			animation_component.animation_data.time_spended++;

			if (animation_component.animation_data.time_spended == 10)
			{
				animation_component.animation_data.step++;
				if (animation_component.animation_data.step > 2) { animation_component.animation_data.step = 0; }

				animation_component.animation_data.time_spended = 0;
			}
			
		}
	}

	void update_sprites_position(stage & stage)
	{
		for (auto & sprite : stage._sprites)
		{
			auto & position_component{ get_component(stage._positions, sprite.id_data) };

			sprite.sprite_data.setPosition(position_component.position_data.x, position_component.position_data.y);
		}
	}

	void update_view(sf::RenderWindow & window, stage & stage, id const& player)
	{
		auto player_position{ get_component(stage._positions, player) };
		auto player_size{ get_component(stage._sizes, player) };

		float screen_width{ static_cast<float>(window.getSize().x) };
		float screen_height{ static_cast<float>(window.getSize().y) };

		float center_x{ player_position.position_data.x + player_size.size_data.width / 2 - screen_width / 2 };
		float center_y{ player_position.position_data.y + player_size.size_data.height / 2 - screen_height / 2 };

		window.setView(sf::View{ sf::FloatRect{center_x, center_y,  screen_width, screen_height } });
	}

	void display_entities(stage & stage, sf::RenderWindow & window)
	{
		for (auto & entity : stage._sprites)
		{
			window.draw(entity.sprite_data);
		}
	}
}

struct texture_pack
{
	sf::Texture pacman;
	sf::Texture point;
	sf::Texture ghost;
};

sf::Texture load_texture(std::string file_path)
{
	sf::Texture texture;
	texture.loadFromFile(file_path);
	return texture;
}

void keyboard_input(ecs::stage & stage, ecs::id target)
{
	float speed{ 3.0f };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ 0, -speed });
		ecs::set_direction(stage, target, ecs::Direction::top );
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ 0, speed });
		ecs::set_direction(stage, target, ecs::Direction::bottom );
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ -speed, 0 });
		ecs::set_direction(stage, target, ecs::Direction::left );
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ speed, 0 });
		ecs::set_direction(stage, target, ecs::Direction::right );
	}
}

int main()
{
	loader loader{};

	loader.load("level1.xml");
	
	map_infos map_infos{ loader.get_map_info() };

	ecs::stage level_1{ map{ map_infos, std::string{ "TileSet.png" } }};

	texture_pack textures{ load_texture("packman-sprite.png"), load_texture("point.png"), load_texture("ghost-sprite.png") };

	std::vector<position> points_positions{ loader.get_points_positions() };
	for (auto const& position : points_positions)
	{
		ecs::add_point(level_1, position, ecs::size{ 48, 48 }, textures.point);
	}

	a_star a_star{ map_infos };

	auto player = ecs::add_mob(level_1, position{ 144, 96 }, ecs::size{ 29, 29 }, textures.pacman);
	ecs::add_animation(level_1, player, ecs::Direction::right, 2);

	ecs::add_mob(level_1, position{ 48, 72 }, ecs::size{ 27, 29 }, textures.ghost);

	sf::RenderWindow window(sf::VideoMode{640 , 480, 32 }, "PacMan");
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

		ecs::update_positions(level_1);
		ecs::update_collisions(level_1, player);

		ecs::update_animations(level_1);
		ecs::update_animations_step(level_1);
		
		ecs::update_sprites_position(level_1);
		ecs::update_view(window, level_1, player);

		window.clear();

		level_1._map.draw_map(window);
		ecs::display_entities(level_1, window);

		window.display();
	}

	return 0;
}