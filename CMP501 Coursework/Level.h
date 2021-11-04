#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include "Tile.h"

class Level
{
	std::vector<std::string> m_levelData;

	std::vector<Tile> m_tiles;
	std::vector<sf::Vector2f> m_playerSpawns;

	bool m_isInit = false;

public:
	Level();
	~Level();

	void Generate(const std::string& filename);

	bool ContainsTerrain(int x, int y);

	void Render(sf::RenderWindow& window);

	sf::Vector2f GetPlayerSpawn(int id);
};

