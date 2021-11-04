#include "Timer.h"

using namespace sf;


Clock Timer::m_gameClock;
Clock Timer::m_frameClock;
Time Timer::m_dt;
Time Timer::m_offset;


void Timer::Start()
{
	m_gameClock.restart();
	m_frameClock.restart();
	m_dt = Time::Zero;
	m_offset = Time::Zero;
}


void Timer::NewFrame()
{
	m_dt = m_frameClock.restart();
}


void Timer::ChangeOffset(float delta)
{
	m_offset += seconds(delta);
}


float Timer::t()
{
	return (m_gameClock.getElapsedTime() + m_offset).asSeconds();
}


float Timer::dt()
{
	return m_dt.asSeconds();
}

