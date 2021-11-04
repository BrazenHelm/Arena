#pragma once

#include <SFML/Graphics.hpp>

class Tile
{
	sf::RectangleShape m_sprite;
	sf::Vector2f m_position;

public:
	Tile(int x, int y);
	~Tile();

	void Render(sf::RenderWindow& window);
};

