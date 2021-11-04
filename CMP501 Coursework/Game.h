#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

#include "Level.h"
#include "NetworkManager.h"
#include "Target.h"

class Input;
class Player;

class Game
{
	const int FRAMERATE_LIMIT = 60;

	sf::RenderWindow	m_window;
	Input*				m_pInput;
	Level				m_level;
	NetworkManager*		m_pNetwork;
	Player*				m_pPlayer;
	Player*				m_pOpponent;

	sf::Clock			m_gameClock;
	sf::Time			m_timeSinceLastUpdate;
	sf::Time			m_timeBetweenUpdates = sf::seconds(0.1f);

	// used to ignore input if the window does not have focus
	bool m_hasFocus = true;

public:
	Game(int width, int height, std::string name, NetworkManager* pNetwork);
	~Game();

	void SetUp();

	// Begin the game
	void Run();

private:
	// Execute everything for one frame
	void DoFrame();

	void HandleWindowEvents();
	void DoInput();
	void ReadNetwork();
	void UpdateEntities();
	void WriteNetwork();
	void Render();
};
