#pragma once

#include <SFML/Network/Packet.hpp>

struct UpdateMessage
{
	// Time
	float t;

	// Position
	float x, y;

	// Velocity
	float vx, vy;

	// Airborne flag
	bool airborne;

	// Health
	int hp;

	// Tells us to start interpolating because something has changed
	bool intX1, intX2, intY;
};

sf::Packet& operator<<(sf::Packet& packet, const UpdateMessage& msg);
sf::Packet& operator>>(sf::Packet& packet, UpdateMessage& msg);
