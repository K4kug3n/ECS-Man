#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>

#include "map.h"

namespace ecs
{
	using id = size_t;
	using entities = std::vector<id>;

	struct position
	{
		float x;
		float y;
	};
	struct position_component
	{
		position position_data;
		id id_data;
	};
	using positions = std::vector<position_component>;

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

	struct stage
	{
		map _map;
		entities _entities;
		positions _positions;
		celerities _celerities;
		types _types;
		sprites _sprites;
	}; 

	template <typename Collection>
	typename Collection::value_type & get_component(Collection & collection, id id)
	{
		auto it = std::find_if(begin(collection), end(collection),
			[id](auto p) {return(p.id_data == id); });

		assert(it != end(collection));

		return (*it);
	}

	id create_entity(stage & stage)
	{
		return stage._entities.empty() ? 1 : stage._entities.size() + 1;;
	}

	id add_mob(stage & stage, position & pos, sf::Texture & texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._positions.push_back(position_component{ pos, id });
		stage._celerities.push_back(celerity_component{ celerity{0, 0}, id });
		stage._types.push_back(type_component{ type::mob, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	id add_point(stage & stage, position & position, sf::Texture & texture)
	{
		const auto id{ create_entity(stage) };
		stage._entities.push_back(id);
		stage._positions.push_back(position_component{ position, id });
		stage._types.push_back(type_component{ type::point, id });
		stage._sprites.push_back(sprite_component{ sprite{ texture }, id });

		return id;
	}

	void remove_entity(stage & stage, id id)
	{
		const auto entities_it{ std::remove(stage._entities.begin(), stage._entities.end(), id) };
		stage._entities.erase(entities_it, stage._entities.end());

		const auto positions_it{ std::find_if(stage._positions.begin(), stage._positions.end(),
			[id](auto p) {return p.id_data == id; }) };

		const auto celerities_it{ std::find_if(stage._celerities.begin(), stage._celerities.end(),
			[id](auto p) {return p.id_data == id; }) };

		const auto types_it{ std::find_if(stage._types.begin(), stage._types.end(),
			[id](auto p) {return p.id_data == id; }) };

		const auto sprites_it{ std::find_if(stage._sprites.begin(), stage._sprites.end(),
			[id](auto p) {return p.id_data == id; }) };
	}

	void print_celerity(stage & stage)
	{
		for (auto & celerity : stage._celerities)
		{
			std::cout << "ID : " << celerity.id_data << "; celerity x : " << celerity.celerity_data.x << "; celerity y : " << celerity.celerity_data.y << std::endl;
		}

	}

	void print_position(stage & stage)
	{
		std::cout << "nb position : " << stage._positions.size() << std::endl;
		for (auto & position : stage._positions)
		{
			std::cout << "ID : " << position.id_data << "; position x : " << position.position_data.x << "; position y : " << position.position_data.y << std::endl;
		}
	}

	void add_celerity(stage & stage, id id, celerity & new_celerity)
	{ 
		auto & celerity_component{ get_component(stage._celerities, id) };

		celerity_component.celerity_data = new_celerity;
	}

	void update_positions(stage & stage)
	{
		for (auto & celerity : stage._celerities)
		{
			auto & position_component{ get_component(stage._positions, celerity.id_data) };
			auto sprite_component{ get_component(stage._sprites, celerity.id_data) };

			int width{ sprite_component.sprite_data.getTextureRect().width };
			int height{ sprite_component.sprite_data.getTextureRect().height };

			float next_x_position{ position_component.position_data.x + celerity.celerity_data.x };
			float next_y_position{ position_component.position_data.y + celerity.celerity_data.y };

			if (!stage._map.check_collision(next_x_position, next_y_position, width, height))
			{
				position_component.position_data.x = next_x_position;
				position_component.position_data.y = next_y_position;
			}

			celerity.celerity_data.x = 0;
			celerity.celerity_data.y = 0;
		}
	}

	void update_sprite_position(stage & stage)
	{
		for (auto & sprite : stage._sprites)
		{
			auto & position_component{ get_component(stage._positions, sprite.id_data) };

			sprite.sprite_data.setPosition(position_component.position_data.x, position_component.position_data.y);
		}
	}

	void update_view(sf::RenderWindow & window, stage & stage, id player)
	{
		auto player_position{ get_component(stage._positions, player) };

		float screen_width{ static_cast<float>(window.getSize().x) };
		float screen_height{ static_cast<float>(window.getSize().y) };

		float center_x{ player_position.position_data.x + 39 / 2 - screen_width / 2 };
		float center_y{ player_position.position_data.y + 39 / 2 - screen_height / 2 };

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
};

sf::Texture load_texture(std::string file_path)
{
	sf::Texture texture;
	texture.loadFromFile(file_path);
	return texture;
}

void keyboard_input(ecs::stage & stage, ecs::id target)
{
	float speed{ 2.0f };

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ 0, -speed });
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ 0, speed });
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ -speed, 0 });
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		ecs::add_celerity(stage, target, ecs::celerity{ speed, 0 });
	}
}

int main()
{
	ecs::stage level_1{ map{ std::string{ "level1.txt" }, std::string{ "TileSet.png" } } };

	texture_pack textures{ load_texture("packman.png"), load_texture("point.png") };

	ecs::add_point(level_1, ecs::position{ 48, 48 }, textures.point);

	auto player = ecs::add_mob(level_1, ecs::position{ 100, 100}, textures.pacman);


	sf::RenderWindow window(sf::VideoMode{640 , 480, 32 }, "PacMan");
	window.setFramerateLimit(60);

	while (window.isOpen()) //Boucle evenement
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
		
		ecs::update_sprite_position(level_1);
		ecs::update_view(window, level_1, player);

		window.clear();

		level_1._map.draw_map(window);
		ecs::display_entities(level_1, window);

		window.display();
	}

	return 0;
}