#include "Game.h"

#include "Input.h"
#include "Player.h"
#include "SpaceConversion.h"
#include "Timer.h"

using namespace sf;


Game::Game(int width, int height, std::string name, NetworkManager* pNetwork) :
	m_window(VideoMode(width, height), name),
	m_pNetwork(pNetwork)
{
	m_window.setFramerateLimit(FRAMERATE_LIMIT);
	m_pInput = new Input(&m_window);
}


Game::~Game()
{
	delete m_pInput;
	delete m_pNetwork;
	delete m_pPlayer;
	delete m_pOpponent;
}


void Game::SetUp()
{
	const std::string LEVEL_DATA_FILENAME = "Assets/Level Data.txt";
	m_level.Generate(LEVEL_DATA_FILENAME);

	bool iAmPlayer0 = (m_pNetwork->GetNetworkID() == 0);

	if (iAmPlayer0)
	{
		m_pPlayer = new Player(m_pInput, &m_level, 0, true);
		m_pOpponent = new Player(m_pInput, &m_level, 1, false);
		m_pPlayer->AddEnemy(m_pOpponent);
		m_pNetwork->AddPlayer(m_pPlayer, 0);
		m_pNetwork->AddPlayer(m_pOpponent, 1);
	}
	else // I am player 1
	{
		m_pPlayer = new Player(m_pInput, &m_level, 1, true);
		m_pOpponent = new Player(m_pInput, &m_level, 0, false);
		m_pPlayer->AddEnemy(m_pOpponent);
		m_pNetwork->AddPlayer(m_pPlayer, 1);
		m_pNetwork->AddPlayer(m_pOpponent, 0);
	}

	m_pPlayer->SetColor(Color(255, 255, 0, 255));		// controlled player is yellow
	m_pOpponent->SetColor(Color(0, 255, 255, 255));		// predicted player is cyan
}


void Game::Run()
{
	Timer::Start();

	while (m_window.isOpen())
	{
		Timer::NewFrame();
		DoFrame();
	}
}


void Game::DoFrame()
{
	HandleWindowEvents();
	DoInput();
	ReadNetwork();
	UpdateEntities();
	WriteNetwork();
	Render();
}


void Game::HandleWindowEvents()
{
	Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == Event::GainedFocus)
		{
			m_hasFocus = true;
		}
		else if (event.type == Event::LostFocus)
		{
			m_hasFocus = false;
		}
	}
}


void Game::DoInput()
{
	m_pInput->NewFrame(m_hasFocus);
}


void Game::ReadNetwork()
{
	m_pNetwork->HandleIncomingMessages();
}


void Game::UpdateEntities()
{
	m_pPlayer->Update();
	m_pOpponent->Update();
	SpaceConversion::SetCameraPosition(m_pPlayer->GetPosition());
}


void Game::WriteNetwork()
{
	m_timeSinceLastUpdate += seconds(Timer::dt());
	bool updateOverride = (m_timeSinceLastUpdate > m_timeBetweenUpdates);
	m_pNetwork->DoOutgoingMessages(updateOverride);
	if (updateOverride)
		m_timeSinceLastUpdate = Time::Zero;
}


void Game::Render()
{
	m_window.clear(Color(0, 0, 0, 255));
	
	m_pPlayer->Render(m_window);
	m_pOpponent->Render(m_window);
	m_level.Render(m_window);
	m_pPlayer->RenderBlood(m_window);

	m_window.display();
}

