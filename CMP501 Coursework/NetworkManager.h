#pragma once

#include <SFML/Network.hpp>
#include <vector>

#include "ShotMessage.h"
#include "UdpConnection.h"
#include "UpdateMessage.h"

class Player;

class NetworkManager
{
	unsigned char m_networkID;				// 0 (host); 1, 2, etc. (clients)
	unsigned char m_nClients;				// only used on host
	unsigned int m_lastUsedPacketID = 0;

	sf::SocketSelector	m_listenerSelect;	// only used on host
	sf::TcpListener		m_listener;			// only used on host

	sf::SocketSelector	m_selector;
	sf::UdpSocket		m_udpSocket;
	sf::TcpSocket		m_tcpSocket;
	UdpConnection		m_udpConnection;

	std::vector<Player*> m_pPlayers;		// vector of players

	float m_timeSinceLastPing = 999.f;
	float m_timeBetweenPings = 5.f;
	float m_timeOfLastPing = 0.f;
	float m_pingTimeout = m_timeBetweenPings / 5.f;

public:
	NetworkManager();
	~NetworkManager();

	void Host();
	void Join();

	void HandleIncomingMessages();
	void DoOutgoingMessages(bool updateOverride);

	void AddPlayer(Player* pPlayer, unsigned char id);

	unsigned char GetNetworkID() { return m_networkID; }

private:
	bool GetHostAddress(std::string& ip, unsigned short& port);

	void ReceiveConnections();

	void SetUpUdpConnection();		// host
	void AcceptUdpConnection();		// client
	sf::Packet CreateUdpConnectionInfo();

	void DealWithPacket(sf::Packet& packet);

	sf::Packet CreateStandardPacket(unsigned char type);
	void SendUpdateMessage(const UpdateMessage& msg);
	void SendTimeRequest();
	void SendTimeReply();
	void SendShotMessage(const ShotMessage& msg);

	// files used to load address information
	const std::string
		HOST_IP_FILENAME = "Network/Host_IP.txt",		// for host IP address
		HOST_PORT_FILENAME = "Network/Host_Port.txt",	// for host TCP listener port
		LOCAL_IP_FILENAME = "Network/Local_IP.txt";		// for client IP address

	const std::string WELCOME_STRING = "No-1isergagsts!";

	enum PacketType
	{
		EntityUpdate, Gunshot, TimeRequest, TimeReply
	};

};
