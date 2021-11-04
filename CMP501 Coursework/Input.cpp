#include "Input.h"

using namespace sf;


Input::Input(Window* pWindow) :
	m_pWindow(pWindow)
{
}


Input::~Input()
{
}


void Input::NewFrame(bool hasFocus)
{
	m_wasHoldingLeft = m_holdingLeft;
	m_wasHoldingRight = m_holdingRight;
	m_wasHoldingJump = m_holdingJump;
	m_prevHorizontal = m_horizontal;

	if (hasFocus)
	{
		m_holdingLeft = Keyboard::isKeyPressed(m_leftKey);
		m_holdingRight = Keyboard::isKeyPressed(m_rightKey);
		m_holdingJump = Keyboard::isKeyPressed(m_jumpKey);
		m_holdingShoot = Mouse::isButtonPressed(m_shootButton);
		m_mousePosition = Mouse::getPosition(*m_pWindow);
	}
	else
	{
		m_holdingLeft = false;
		m_holdingRight = false;
		m_holdingJump = false;
		m_holdingShoot = false;
	}

	if (m_holdingLeft && !m_holdingRight)
		m_horizontal = -1.f;
	else if (m_holdingRight && !m_holdingLeft)
		m_horizontal = 1.f;
	else
		m_horizontal = 0.f;
}


bool Input::StartedHorizontal()
{
	return (m_horizontal != 0.f && m_horizontal != m_prevHorizontal);
}


bool Input::StoppedHorizontal()
{
	return (m_horizontal == 0.f && m_prevHorizontal != 0.f);
}


bool Input::JustJumped()
{
	return (m_holdingJump && !m_wasHoldingJump);
}

