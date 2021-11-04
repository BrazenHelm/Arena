#include "Gun.h"

#include "Level.h"
#include "Player.h"
#include "SpaceConversion.h"
#include "Timer.h"

using namespace sf;


Gun::Gun(Level* pLevel) :
	m_pLevel(pLevel),
	m_fireRate(3.f),
	m_isTriggerHeld(false),
	m_timeSinceLastShot(999.f)
{
}


Gun::~Gun()
{
}


void Gun::AddEnemy(Player* pEnemy)
{
	m_pEnemies.push_back(pEnemy);
}


bool Gun::Update(bool controlled)
{
	m_timeSinceLastShot += Timer::dt();

	if (!controlled) return false;
	if (!m_isTriggerHeld) return false;

	if (m_timeSinceLastShot >= 1.f / m_fireRate)
	{
		Shoot();
		return true;
	}
	
	return false;
}


void Gun::Render(RenderWindow& window)
{
	if (m_timeSinceLastShot < TIME_FOR_TRACER_FADE)
	{
		tracer[0] = Vertex(WorldToScreenPosition(m_positionAtLastShot));
		tracer[1] = Vertex(WorldToScreenPosition(Vector2f(m_lastShot.x, m_lastShot.y)));
		window.draw(tracer, 2, Lines);
	}
}


void Gun::SetLastShot(const ShotMessage& msg)
{
	m_lastShot = msg;
	m_timeSinceLastShot = 0;
	m_positionAtLastShot = m_position;
}


void Gun::Shoot()
{
	m_timeSinceLastShot = 0.f;

	Ray ray;
	ray.origin = m_position;
	ray.dir = Vector2f(cosf(m_aimAngle), sinf(m_aimAngle));

	// after all the loops, this will be the distance at which a collision occurred
	float t = 0.f;

	// cell of start point of ray
	int xori = static_cast<int>(floorf(ray.origin.x));
	int yori = static_cast<int>(floorf(ray.origin.y));

	// we only check quadrant in direction of ray
	int xdir = (ray.dir.x < 0.f) ? -1 : 1;
	int ydir = (ray.dir.y < 0.f) ? -1 : 1;

	bool collisionFound = false;

	const int MAX_DISTANCE_TO_CHECK = 100;

	// look for collision with terrain
	for (int distance = 1; distance < MAX_DISTANCE_TO_CHECK; distance++)
	{
		// We check the cells working outwards from the origin point x
		// e.g. shooting down and right we check in order:
		// x 1 4
		// 2 3 5
		// 6 7 8 etc.

		// new column
		for (int i = 0; i < distance; i++)
		{
			int x = xori + xdir * distance;
			int y = yori + ydir * i;

			collisionFound = CheckCellCollision(ray, x, y, xdir, ydir, t);
			if (collisionFound) break;
		}
		if (collisionFound) break;

		// new row
		for (int j = 0; j <= distance; j++)
		{
			int x = xori + xdir * j;
			int y = yori + ydir * distance;

			collisionFound = CheckCellCollision(ray, x, y, xdir, ydir, t);
			if (collisionFound) break;
		}
		if (collisionFound) break;
	}
	// now t is the distance at which a collision was found

	m_positionAtLastShot = m_position;
	m_lastShot.playerHit = -1;
	m_lastShot.x = m_position.x + t * ray.dir.x;
	m_lastShot.y = m_position.y + t * ray.dir.y;

	float dist = t;

	for (Player* pPlayer : m_pEnemies)
	{
		Circle circle;
		circle.centre = pPlayer->GetPosition();
		circle.radius = pPlayer->GetSize();

		if (CheckCollision(ray, circle, dist))
		{
			if (dist < t)
			{
				m_lastShot.playerHit = pPlayer->GetID();
				m_lastShot.x = m_position.x + dist * ray.dir.x;
				m_lastShot.y = m_position.y + dist * ray.dir.y;
			}
		}
	}
}


bool Gun::CheckCellCollision(Ray ray, int x, int y, int xdir, int ydir, float& t)
{
	if (!m_pLevel->ContainsTerrain(x, y))
		return false;

	Vector2f nearCorner;
	nearCorner.x = (xdir == 1) ? x : x + 1;
	nearCorner.y = (ydir == 1) ? y : y + 1;
	Vector2f farCorner;
	farCorner.x = (xdir == 1) ? x + 1 : x;
	farCorner.y = (ydir == 1) ? y + 1 : y;

	LineSegment v;
	v.start = nearCorner;
	v.end.x = nearCorner.x;
	v.end.y = farCorner.y;

	if (CheckCollision(ray, v, t))
		return true;

	LineSegment h;
	h.start = nearCorner;
	h.end.x = farCorner.x;
	h.end.y = nearCorner.y;

	if (CheckCollision(ray, h, t))
		return true;
	
	return false;
}

