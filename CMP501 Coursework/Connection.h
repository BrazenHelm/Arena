#pragma once

#include <SFML/Network.hpp>

class Connection
{
	// The UDP socket used to send data to the connection
	sf::UdpSocket* m_pSocket;

	// The IP address of the connection
	sf::IpAddress m_ip;

	// The port number of the connection
	unsigned short m_port;

public:
	Connection(sf::UdpSocket* pSocket, std::string ip, unsigned short port);
	~Connection();

	void Accept();
	void Refuse();

	void Send(sf::Packet packet);
};

