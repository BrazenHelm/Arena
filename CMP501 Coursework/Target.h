#pragma once

#include <SFML/Graphics.hpp>

class Target
{
	sf::CircleShape m_sprite;
	sf::Vector2f m_position;
	float m_size = 0.5f;

	float m_timeToReset = 2.f;
	float m_timeUntilReset = 0.f;

public:
	Target(float x, float y);
	~Target();

	sf::Vector2f GetPosition() { return m_position; }
	float GetSize() { return m_size; }

	void Update();

	void Render(sf::RenderWindow& window);

	void Hit();
};

