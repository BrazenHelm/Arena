#pragma once

#include <SFML/Graphics.hpp>

class Input
{
	sf::Keyboard::Key
		m_leftKey	= sf::Keyboard::A,
		m_rightKey	= sf::Keyboard::D,
		m_jumpKey	= sf::Keyboard::W;

	sf::Mouse::Button
		m_shootButton = sf::Mouse::Button::Left;

	bool
		m_holdingLeft = false,
		m_holdingRight = false,
		m_holdingJump = false,
		m_holdingShoot = false;

	bool
		m_wasHoldingLeft = false,
		m_wasHoldingRight = false,
		m_wasHoldingJump = false;

	float m_horizontal;
	float m_prevHorizontal;
	sf::Vector2i m_mousePosition;

	sf::Window* m_pWindow;

public:
	Input(sf::Window* pWindow);
	~Input();

	void NewFrame(bool hasFocus);

	float GetHorizontal()			{ return m_horizontal; }
	bool GetJump()					{ return m_holdingJump; }
	bool GetShoot()					{ return m_holdingShoot; }
	sf::Vector2i GetMousePosition() { return m_mousePosition; }
	
	bool JustJumped();
	bool StartedHorizontal();
	bool StoppedHorizontal();
};

