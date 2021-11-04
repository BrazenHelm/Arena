#pragma once

#include <SFML/Network/Packet.hpp>

struct ShotMessage
{
	// position of first collision
	float x, y;

	int playerHit = -1;	// -1 for no player hit, else use their network ID
	int damage;
};

sf::Packet& operator<<(sf::Packet& packet, const ShotMessage& msg);
sf::Packet& operator>>(sf::Packet& packet, ShotMessage& msg);

