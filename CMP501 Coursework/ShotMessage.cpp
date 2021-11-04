#include "ShotMessage.h"

using namespace sf;


Packet& operator<<(Packet& packet, const ShotMessage& msg)
{
	return packet << msg.x << msg.y << msg.playerHit << msg.damage;
}


Packet& operator>>(Packet& packet, ShotMessage& msg)
{
	return packet >> msg.x >> msg.y >> msg.playerHit >> msg.damage;
}
