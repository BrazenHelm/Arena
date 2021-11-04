#include "Tile.h"

#include "SpaceConversion.h"


using namespace sf;

Tile::Tile(int x, int y)
{
	float sideLength = WorldToScreenLength(1.f);
	m_sprite.setSize(Vector2f(sideLength, sideLength));

	m_position.x = x;
	m_position.y = y + 1;	// because origin is in top left

	if ((x + y) % 2 == 0)
	{
		m_sprite.setFillColor(Color(100, 100, 100, 255));
	}
	else
	{
		m_sprite.setFillColor(Color(130, 130, 130, 255));
	}
}


Tile::~Tile()
{
}


void Tile::Render(RenderWindow& window)
{
	m_sprite.setPosition(WorldToScreenPosition(m_position));
	window.draw(m_sprite);
}