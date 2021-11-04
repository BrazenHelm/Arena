#pragma once

#include <SFML/Graphics.hpp>

#include "Gun.h"
#include "ShotMessage.h"
#include "UpdateMessage.h"

class Input;
class Level;

class Player
{
	enum MovementType
	{
		Controlled, Predicted
	};

	enum PredictionMode
	{
		None, Linear, LinearInterpolate, Advanced, AdvancedInterpolate
	};

	MovementType m_movementType;
	PredictionMode m_predictionMode = AdvancedInterpolate;
	int m_id;

	Input* m_pInput;
	Level* m_pLevel;

	Gun m_gun;

	sf::CircleShape m_bodySprite;

	sf::RectangleShape m_bloodOverlay;
	const float BLOOD_VISIBLE_TIME = 1.f;
	float m_timeSinceDeath = 999.f;

	float m_jumpPower = 12.f;
	float m_moveSpeed = 5.f;
	float m_size = 0.5f;

	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
	bool m_isAirborne = true;
	bool m_wasAirborne = false;
	float m_aimAngle;

	bool m_wantsToSendUpdate = false;
	bool m_tellFastInterpolateX = false;
	bool m_tellSlowInterpolateX = false;
	bool m_tellInterpolateY = false;
	bool m_unsentShot = false;

	sf::Vector2f m_predictedPos;
	sf::Vector2f m_posAtPredictionChange;

	UpdateMessage m_lastUpdate;
	UpdateMessage m_predictionPoint;
	bool m_doSlowInterpolate = false;
	sf::Vector2f m_interpolationTimes = sf::Vector2f(999.f, 999.f);

	const float FAST_CATCH_UP = 0.2f;
	const float SLOW_CATCH_UP = 2.f;

	const float GRAVITY = -20.f;

public:
	Player(Input* pInput, Level* pLevel, int id, bool controlled);
	~Player();

	void AddEnemy(Player* pEnemy);

	void Update();
	void Render(sf::RenderWindow& window);
	void RenderBlood(sf::RenderWindow& window);

	void TakeMessage(const UpdateMessage& msg);
	void YouShot(const ShotMessage& msg);
	void YouWereShot(int damage);

	UpdateMessage CurrentStatus();
	ShotMessage LastShot();

	bool WantsToSendUpdate() { return m_wantsToSendUpdate; }
	bool UnsentShot() { return m_unsentShot; }

	bool IsControlled() { return m_movementType == Controlled; }
	sf::Vector2f GetPosition() { return m_position; }
	float GetSize() { return m_size; }
	int GetID() { return m_id; }

	void SetColor(sf::Color color) { m_bodySprite.setFillColor(color); }

private:
	void CalculatePosition();
	void DoGunShooting();

	sf::Vector2f PredictPosition(const UpdateMessage& point, float time);
	sf::Vector2f PredictCollision(sf::Vector2f& pos, UpdateMessage& point, float time);

	sf::Vector2f PredictLinear(const UpdateMessage& point, float time);
	sf::Vector2f PredictAdvanced(const UpdateMessage& point, float time);

	void RetroactivePrediction(UpdateMessage& point);

	std::vector<float> CollideWithTerrain(float x, float y);

	void Respawn();
};

