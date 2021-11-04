#include "Target.h"

#include "SpaceConversion.h"
#include "Timer.h"

using namespace sf;

Target::Target(float x, float y)
{
	m_position.x = x;
	m_position.y = y;

	float screenSize = WorldToScreenLength(m_size);
	m_sprite.setRadius(screenSize);
	m_sprite.setOrigin(screenSize, screenSize);
	m_sprite.setFillColor(Color(0, 255, 0, 255));
}


Target::~Target()
{
}


void Target::Update()
{
	m_timeUntilReset -= Timer::dt();
	
	int color = 0;

	if (m_timeUntilReset > 0.f)
	{
		color = static_cast<int>(floorf(m_timeUntilReset / m_timeToReset * 255.f));
	}

	m_sprite.setFillColor(Color(color, 255 - color, 0, 255));

	m_sprite.setPosition(WorldToScreenPosition(m_position));
}


void Target::Render(RenderWindow& window)
{
	window.draw(m_sprite);
}


void Target::Hit()
{
	printf("Target hit\n");
	m_timeUntilReset = m_timeToReset;
}