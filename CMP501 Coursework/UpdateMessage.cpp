#include "UpdateMessage.h"

using namespace sf;


Packet& operator<<(Packet& packet, const UpdateMessage& msg)
{
	return packet << msg.t << msg.x << msg.y << msg.vx << msg.vy << msg.airborne << msg.hp
		<< msg.intX1 << msg.intX2 << msg.intY;
}


Packet& operator>>(Packet& packet, UpdateMessage& msg)
{
	return packet >> msg.t >> msg.x >> msg.y >> msg.vx >> msg.vy >> msg.airborne >> msg.hp
		>> msg.intX1 >> msg.intX2 >> msg.intY;
}
