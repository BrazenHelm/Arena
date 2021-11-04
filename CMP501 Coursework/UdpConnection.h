#pragma once

#include <SFML/Network.hpp>
#include <string>

class UdpConnection
{
	sf::UdpSocket*	m_pSocket = nullptr;
	std::string		m_ip;
	unsigned short	m_port;

	bool m_init = false;

public:
	UdpConnection();
	~UdpConnection();

	void Initialise(sf::UdpSocket* pSocket, std::string ip, unsigned short port);
	void Initialise(sf::UdpSocket* pSocket, sf::Packet address);

	void Send(sf::Packet packet);
	void Receive(sf::Packet& packet);
};

