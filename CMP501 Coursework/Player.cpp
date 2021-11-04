#include "Player.h"

#include "Level.h"
#include "Input.h"
#include "SpaceConversion.h"
#include "Timer.h"

using namespace sf;

Player::Player(Input* pInput, Level* pLevel, int id, bool controlled) :
	m_pInput(pInput),
	m_pLevel(pLevel),
	m_gun(pLevel),
	m_id(id)
{
	m_position = m_pLevel->GetPlayerSpawn(id);
	//printf("Creating player at (%.0f, %.0f), id %i\n", m_position.x, m_position.y, id);
	m_movementType = (controlled) ? Controlled : Predicted;

	m_predictionPoint.x = m_position.x;
	m_predictionPoint.y = m_position.y;
	m_predictionPoint.airborne = true;

	float screenSize = WorldToScreenLength(m_size);
	m_bodySprite = CircleShape(screenSize);
	m_bodySprite.setOrigin(screenSize, screenSize);
	SetColor(Color(255, 255, 255, 255));

	//m_gunSprite = RectangleShape(Vector2f(screenSize * 2.f, screenSize / 2.5f));
	//m_gunSprite.setOrigin(screenSize, screenSize / 5.f);
	//m_gunSprite.setFillColor(Color(255, 0, 0, 255));

	m_bloodOverlay = RectangleShape(Vector2f(
		SpaceConversion::SCREEN_WIDTH,
		SpaceConversion::SCREEN_HEIGHT
	));
}


Player::~Player()
{
}


void Player::AddEnemy(Player* pEnemy)
{
	m_gun.AddEnemy(pEnemy);
}


void Player::Update()
{
	//m_interpolationTime += Timer::dt();
	//m_timeThisFrame = Timer::t();

	m_interpolationTimes.x += Timer::dt();
	m_interpolationTimes.y += Timer::dt();

	m_timeSinceDeath += Timer::dt();

	switch (m_movementType)
	{
		case Controlled:
		{
			CalculatePosition();

			std::vector<float> depths = CollideWithTerrain(m_position.x, m_position.y);
			m_position.x += (depths[0] - depths[1]);
			m_position.y += (depths[2] - depths[3]);
			m_wasAirborne = m_isAirborne;
			m_isAirborne = !(depths[2] > 0.f);
			if (depths[3] > 0.f) m_velocity.y = 0.f;

			DoGunShooting();
			break;
		}
		case Predicted:
		{
			m_predictedPos = PredictPosition(m_predictionPoint, Timer::t());
			m_position = PredictCollision(m_predictedPos, m_predictionPoint, Timer::t());
			m_gun.SetPosition(m_position);
			m_gun.Update(false);
			break;
		}
		default:
		{
			printf("Player::Update - invalid movement mode selected\n");
			break;
		}
	}
}


void Player::Render(RenderWindow& window)
{
	m_bodySprite.setPosition(WorldToScreenPosition(m_position));
	window.draw(m_bodySprite);
	m_gun.Render(window);
}


void Player::RenderBlood(RenderWindow& window)
{
	int bloodAlpha = 0;
	if (m_timeSinceDeath < BLOOD_VISIBLE_TIME)
	{
		bloodAlpha = 255 - static_cast<int>(floorf(255.f * m_timeSinceDeath / BLOOD_VISIBLE_TIME));
	}
	m_bloodOverlay.setFillColor(Color(255, 0, 0, bloodAlpha));
	window.draw(m_bloodOverlay);
}


void Player::TakeMessage(const UpdateMessage& msg)
{
	if (msg.t > m_lastUpdate.t)
	{
		m_lastUpdate = msg;
		m_predictionPoint = msg;
		RetroactivePrediction(m_predictionPoint);

		if (m_lastUpdate.intX1)
		{
			m_posAtPredictionChange.x = m_position.x;
			m_interpolationTimes.x = 0.f;
			m_doSlowInterpolate = false;
		}
		if (m_lastUpdate.intX2)
		{
			m_posAtPredictionChange.x = m_position.x;
			m_interpolationTimes.x = 0.f;
			m_doSlowInterpolate = true;
		}
		if (m_lastUpdate.intY)
		{
			m_posAtPredictionChange.y = m_position.y;
			m_interpolationTimes.y = 0.f;
		}
	}
}


void Player::RetroactivePrediction(UpdateMessage& point)
{
	const float TIMESTEP = 0.05f;
	for (float t = point.t; t < Timer::t(); t += TIMESTEP)
	{
		m_predictedPos = PredictPosition(point, t);
		PredictCollision(m_predictedPos, point, t);
	}
}


void Player::YouShot(const ShotMessage& msg)
{
	m_gun.SetLastShot(msg);
}


void Player::YouWereShot(int damage)
{
	m_timeSinceDeath = 0.f;
	Respawn();
}


void Player::Respawn()
{
	m_position = m_pLevel->GetPlayerSpawn(m_id);
}


UpdateMessage Player::CurrentStatus()
{
	UpdateMessage status;

	status.t = Timer::t();
	status.x = m_position.x;
	status.y = m_position.y;
	status.vx = m_velocity.x;
	status.vy = m_velocity.y;
	status.airborne = m_isAirborne;
	status.hp = 1;
	status.intX1 = m_tellFastInterpolateX;
	status.intX2 = m_tellSlowInterpolateX;
	status.intY = m_tellInterpolateY;

	m_tellFastInterpolateX = false;
	m_tellSlowInterpolateX = false;
	m_tellInterpolateY = false;
	m_wantsToSendUpdate = false;

	return status;
}


ShotMessage Player::LastShot()
{
	m_unsentShot = false;
	return m_gun.LastShot();
}


void Player::CalculatePosition()
{
	if (m_pInput->StartedHorizontal())
	{
		m_tellFastInterpolateX = true;
		m_wantsToSendUpdate = true;
	}
	if (m_pInput->StoppedHorizontal())
	{
		m_tellSlowInterpolateX = true;
		m_wantsToSendUpdate = true;
	}
	if (m_pInput->JustJumped())
	{
		m_tellInterpolateY = true;
		m_wantsToSendUpdate = true;
	}

	m_velocity.x = m_pInput->GetHorizontal() * m_moveSpeed;

	if (m_isAirborne)
	{
		m_velocity.y += GRAVITY * Timer::dt();
	}
	else if (m_pInput->GetJump())
	{
		m_velocity.y = m_jumpPower;
	}
	else
	{
		m_velocity.y = 0.f;
	}

	if (Timer::dt() < 0.1f)		// ignore frames where window was being dragged to avoid glitches
	{
		m_position += m_velocity * Timer::dt();
	}
}


void Player::DoGunShooting()
{
	Vector2i mousePos = m_pInput->GetMousePosition();
	Vector2f mouseWorldPos = ScreenToWorldPosition(Vector2f(mousePos.x, mousePos.y));
	Vector2f aimDir = mouseWorldPos - m_position;
	m_aimAngle = atan2f(aimDir.y, aimDir.x);

	if (m_pInput->GetShoot())
		m_gun.TriggerPulled();
	else
		m_gun.TriggerReleased();

	m_gun.SetAimAngle(m_aimAngle);
	m_gun.SetPosition(m_position);

	m_unsentShot = m_gun.Update(true);
}


Vector2f Player::PredictPosition(const UpdateMessage& point, float time)
{
	Vector2f result;

	switch (m_predictionMode)
	{
		case None:
		{
			result.x = m_lastUpdate.x;
			result.y = m_lastUpdate.y;
			break;
		}
		case Linear: case LinearInterpolate:
		{
			result = PredictLinear(point, time);
			break;
		}
		case Advanced: case AdvancedInterpolate:
		{
			result = PredictAdvanced(point, time);
			break;
		}
		default:
		{
			printf("Player::PredictPosition - invalid prediction mode selected\n");
			break;
		}
	}

	return result;
}


Vector2f Player::PredictCollision(Vector2f& pos, UpdateMessage& point, float time)
{
	std::vector<float> depths = CollideWithTerrain(m_predictedPos.x, m_predictedPos.y);

	if (m_predictionMode != None)
	{
		pos.x += (depths[0] - depths[1]);
		pos.y += (depths[2] - depths[3]);
		point.x += (depths[0] - depths[1]);
		point.y += (depths[2] - depths[3]);
		if (depths[3] > 0.f)
		{
			point.t = time;
			point.x = m_predictedPos.x;
			point.y = m_predictedPos.y;
			point.vy = 0.f;
		}
		if (depths[2] > 0.f && m_predictionPoint.airborne)
		{
			point.t = time;
			point.x = m_predictedPos.x;
			point.y = m_predictedPos.y;
			point.vy = 0.f;
			point.airborne = false;
		}
		if (depths[2] == 0.f && !m_predictionPoint.airborne)
		{
			point.t = time;
			point.x = m_predictedPos.x;
			point.y = m_predictedPos.y;
			point.airborne = true;
		}
	}

	if (m_predictionMode == LinearInterpolate || m_predictionMode == AdvancedInterpolate)
	{
		float cutx = (m_doSlowInterpolate) ? SLOW_CATCH_UP : FAST_CATCH_UP;
		float tx = m_interpolationTimes.x / cutx;
		float ty = m_interpolationTimes.y / FAST_CATCH_UP;

		if (tx < 1)
		{
			pos.x = (1 - tx) * m_posAtPredictionChange.x + tx * pos.x;
		}
		if (ty < 1)
		{
			pos.y = (1 - ty) * m_posAtPredictionChange.y + ty * pos.y;
		}
	}

	return pos;
}


Vector2f Player::PredictLinear(const UpdateMessage& point, float time)
{
	float t = time - point.t;
	float x = point.x + point.vx * t;
	float y = point.y + point.vy * t;
	return Vector2f(x, y);
}


Vector2f Player::PredictAdvanced(const UpdateMessage& point, float time)
{
	float t = time - point.t;
	float x = point.x + point.vx * t;
	float y = point.y + point.vy * t;
	if (point.airborne)
		y += 0.5f * GRAVITY * t * t;

	return Vector2f(x, y);
}


std::vector<float> Player::CollideWithTerrain(float x, float y)
{
	std::vector<float> depths;

	int xl = static_cast<int>(floorf(x - m_size));
	int xc = static_cast<int>(floorf(x));
	int xr = static_cast<int>(floorf(x + m_size));
	int yb = static_cast<int>(floorf(y - m_size));
	int yc = static_cast<int>(floorf(y));
	int yt = static_cast<int>(floorf(y + m_size));

	// collision on left
	if (m_pLevel->ContainsTerrain(xl, yc))
	{
		float depth = xl + 1.f - x + m_size;
		depths.push_back(depth);
	}
	else
	{
		depths.push_back(0.f);
	}

	// collision on right
	if (m_pLevel->ContainsTerrain(xr, yc))
	{
		float depth = x + m_size - xr;
		depths.push_back(depth);
	}
	else
	{
		depths.push_back(0.f);
	}

	// collision below
	if (m_pLevel->ContainsTerrain(xc, yb))
	{
		float depth = yb + 1.f - y + m_size;
		depths.push_back(depth);
	}
	else
	{
		depths.push_back(0.f);
	}

	// collision above
	if (m_pLevel->ContainsTerrain(xc, yt))
	{
		float depth = y + m_size - yt;
		depths.push_back(depth);
	}
	else
	{
		depths.push_back(0.f);
	}

	return depths;
}

