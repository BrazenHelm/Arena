#pragma once

#include <SFML/System/Clock.hpp>

class Timer
{
	static sf::Clock m_gameClock;
	static sf::Clock m_frameClock;

	static sf::Time m_dt;
	static sf::Time m_offset;

public:
	static void Start();
	static void NewFrame();

	static void ChangeOffset(float delta);

	// Time elapsed since start of game in seconds
	static float t();

	// Time last frame took to execute in seconds
	static float dt();
};

