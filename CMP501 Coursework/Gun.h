#pragma once

// This class simply determines when and in what direction the gun is fired
// Input and output is done by the Player class

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "Collisions.h"
#include "ShotMessage.h"
#include "Target.h"

class Level;
class Player;


class Gun
{
	Level* m_pLevel;	// for hitscan calculations

	float m_fireRate;

	bool m_isTriggerHeld;
	float m_timeSinceLastShot;

	sf::Vector2f m_position;
	float m_aimAngle;

	std::vector<Player*> m_pEnemies;

	ShotMessage		m_lastShot;
	sf::Vector2f	m_positionAtLastShot;
	const float		TIME_FOR_TRACER_FADE = 0.05f;
	sf::Vertex		tracer[2];

public:
	Gun(Level* pLevel);
	~Gun();

	void AddEnemy(Player* pEnemy);

	void TriggerPulled() { m_isTriggerHeld = true; }
	void TriggerReleased() { m_isTriggerHeld = false; }

	bool Update(bool controlled);	// returns true if the gun shot this frame
	void Render(sf::RenderWindow& window);

	void SetAimAngle(float angle) { m_aimAngle = angle; }
	float GetAimAngle() { return m_aimAngle; }

	void SetPosition(sf::Vector2f position) { m_position = position; }
	sf::Vector2f GetPosition() { return m_position; }
	ShotMessage LastShot() { return m_lastShot; }
	void SetLastShot(const ShotMessage& msg);

private:
	void Shoot();

	// Checks collision of given ray with cell at (x, y)
	// t becomes the distance along the ray at which the collision occurred
	bool CheckCellCollision(Ray ray, int x, int y, int xdir, int ydir, float& t);
};

