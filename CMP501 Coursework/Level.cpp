#include "Level.h"

#include <fstream>

using namespace sf;


Level::Level()
{
	m_playerSpawns.resize(2);
}


Level::~Level()
{
}


void Level::Generate(const std::string& filename)
{
	printf("making a level\n");
	std::ifstream file;
	file.open(filename);
	if (file.is_open())
	{
		std::string buffer;
		while (std::getline(file, buffer))
		{
			m_levelData.push_back(buffer);
		}
		file.close();
	}
	else
	{
		printf("Level::Generate - could not open file\n");
		return;
	}

	for (size_t y = 0; y < m_levelData.size(); y++)
	{
		size_t worldY = m_levelData.size() - 1 - y;
		for (size_t x = 0; x < m_levelData[y].size(); x++)
		{
			char tileData = m_levelData[y][x];

			switch (tileData)
			{
			case 'x':			// terrain
				//printf("Making a tile at (%i, %i)\n", x, worldY);
				m_tiles.emplace_back(x, worldY);
				break;

			case '0':			// player spawn
				m_playerSpawns[0].x = x;
				m_playerSpawns[0].y = worldY;
				break;

			case '1':			// player spawn
				m_playerSpawns[1].x = x;
				m_playerSpawns[1].y = worldY;
				break;

			default:			// empty
				break;
			}
		}
	}

	m_isInit = true;
}


bool Level::ContainsTerrain(int x, int y)
{
	if (!m_isInit)
	{
		printf("Level::ContainsTerrain - looked for terrain in uninitialised level\n");
		return false;
	}

	y = m_levelData.size() - 1 - y;
	if (y < 0 || y >= m_levelData.size() || x < 0 || x >= m_levelData[y].size())
	{
		// out of bounds
		return false;
	}
	else
	{
		return (m_levelData[y][x] == 'x');
	}
}


void Level::Render(sf::RenderWindow& window)
{
	if (!m_isInit)
	{
		printf("Level::Render - tried to render uninitialised level\n");
		return;
	}

	for (Tile tile : m_tiles)
	{
		tile.Render(window);
	}
}


Vector2f Level::GetPlayerSpawn(int id)
{
	return m_playerSpawns[id] + Vector2f(0.5f, 0.5f);	// offset to put player in middle of cell
}

