#include "NetworkManager.h"

#include <SFML/Window/Keyboard.hpp>
#include <fstream>
#include <iostream>

#include "Player.h"
#include "Timer.h"
#include "UpdateMessage.h"

using namespace sf;


NetworkManager::NetworkManager()
{
	m_pPlayers.resize(2);
	m_pPlayers[0] = nullptr;
	m_pPlayers[1] = nullptr;
}


NetworkManager::~NetworkManager()
{
	m_tcpSocket.disconnect();
}


void NetworkManager::Host()
{
	printf("Hosting...\n");

	if (m_listener.listen(Socket::AnyPort) != Socket::Done)
	{
		printf("NetworkManager::Host - Could not bind TCP listener\n");
		return;
	}

	unsigned short port = m_listener.getLocalPort();

	printf("Writing port information (%i) in file\n", port);

	std::ofstream file;
	file.open(HOST_PORT_FILENAME);
	if (file.is_open())
	{
		file << port;
		file.close();
	}
	else
	{
		printf("Error opening file\n");
		return;
	}

	m_networkID = 0;
	m_nClients = 0;

	printf("Waiting for others to join. At any point, press S to start.\n");

	m_listenerSelect.add(m_listener);
	bool inMenuMode = true;


	while (inMenuMode)
	{
		ReceiveConnections();
		if (Keyboard::isKeyPressed(Keyboard::Key::S))
		{
			inMenuMode = false;
		}
	}

	printf("Starting game...\n");

	SetUpUdpConnection();
	// SynchroniseWatches();
}


void NetworkManager::Join()
{
	printf("Joining...\n");

	std::string hostIP;
	unsigned short hostPort;

	if (!GetHostAddress(hostIP, hostPort))
	{
		printf("NetworkManager::Join - Failed to retrieve host address\n");
		return;
	}

	std::cout << "Connecting to Host: IP " << hostIP << ", port " << hostPort << std::endl;

	if (m_tcpSocket.connect(hostIP, hostPort) != Socket::Done)
	{
		printf("NetworkManager::Join - Failed to connect to host\n");
		return;
	}
	m_selector.add(m_tcpSocket);

	Packet packet;
	std::string recvdMsg;
	m_tcpSocket.receive(packet);
	packet >> recvdMsg >> m_networkID;

	if (recvdMsg == WELCOME_STRING)
	{
		printf("Connection successfully established\n");
		printf("Assigned ID is %i\n", m_networkID);
	}
	else
	{
		printf("NetworkManager::Join - Unexpected welcome message received\n");
		return;
	}

	printf("Waiting for game to start...\n");

	AcceptUdpConnection();
}


void NetworkManager::HandleIncomingMessages()
{
	while (m_selector.wait(microseconds(1)))	// almost immediate timeout
	{
		Packet incoming;

		if (m_selector.isReady(m_tcpSocket))
		{
			m_tcpSocket.receive(incoming);
		}
		else if (m_selector.isReady(m_udpSocket))
		{
			m_udpConnection.Receive(incoming);
		}

		DealWithPacket(incoming);
	}
}


void NetworkManager::DoOutgoingMessages(bool updateOverride)
{
	for (Player* pPlayer : m_pPlayers)
	{
		if (pPlayer->IsControlled() && (updateOverride || pPlayer->WantsToSendUpdate()))
		{
			SendUpdateMessage(pPlayer->CurrentStatus());
		}
		if (pPlayer->IsControlled() && pPlayer->UnsentShot())
		{
			SendShotMessage(pPlayer->LastShot());
		}
	}

	m_timeSinceLastPing += Timer::dt();
	if (m_networkID != 0 && m_timeSinceLastPing > m_timeBetweenPings)
	{
		SendTimeRequest();
		m_timeSinceLastPing = 0.f;
	}
}


void NetworkManager::AddPlayer(Player* pPlayer, unsigned char id)
{
	if (id > 1)
	{
		printf("NetworkManager::AddPlayer - tried to add player with id too large (%i)\n", id);
		return;
	}
	
	m_pPlayers[id] = pPlayer;
	//printf("Successfully added player (id %i) to network\n", id);
}


bool NetworkManager::GetHostAddress(std::string& ip, unsigned short& port)
{
	std::ifstream hostIPFile;
	hostIPFile.open(HOST_IP_FILENAME);
	if (hostIPFile.is_open())
	{
		hostIPFile >> ip;
		hostIPFile.close();
	}
	else
	{
		printf("NetworkManager::GetHostAddress - Error opening IP file\n");
		return false;
	}

	std::ifstream hostPortFile;
	hostPortFile.open(HOST_PORT_FILENAME);
	if (hostPortFile.is_open())
	{
		hostPortFile >> port;
		hostPortFile.close();
	}
	else
	{
		printf("NetworkManager::GetHostAddress - Error opening port file\n");
		return false;
	}
	return true;
}


void NetworkManager::SetUpUdpConnection()
{
	// bind udp socket to a port
	m_udpSocket.bind(Socket::AnyPort);
	m_selector.add(m_udpSocket);

	// send udp address to client
	Packet myAddress = CreateUdpConnectionInfo();
	m_tcpSocket.send(myAddress);

	// receive client udp address
	Packet clientAddress;
	m_tcpSocket.receive(clientAddress);
	m_udpConnection.Initialise(&m_udpSocket, clientAddress);
}


void NetworkManager::AcceptUdpConnection()
{
	// bind udp socket to a port
	m_udpSocket.bind(Socket::AnyPort);
	m_selector.add(m_udpSocket);

	// receive host udp address
	Packet hostAddress;
	m_tcpSocket.receive(hostAddress);
	m_udpConnection.Initialise(&m_udpSocket, hostAddress);

	// send udp address to host
	Packet myAddress = CreateUdpConnectionInfo();
	m_tcpSocket.send(myAddress);
}


Packet NetworkManager::CreateUdpConnectionInfo()
{
	unsigned short udpPort = m_udpSocket.getLocalPort();

	std::string udpIP;
	std::string ipFilename = (m_networkID == 0) ? HOST_IP_FILENAME : LOCAL_IP_FILENAME;
	std::ifstream ipFile;
	ipFile.open(ipFilename);
	if (ipFile.is_open())
	{
		ipFile >> udpIP;
		ipFile.close();
	}
	else
	{
		printf("NetworkManager::CreateUdpConnectionInfo - could not open file\n");
	}

	Packet udpConnectionInfo;
	udpConnectionInfo << udpIP << udpPort;
	return udpConnectionInfo;
}


void NetworkManager::ReceiveConnections()
{
	if (m_listenerSelect.wait(microseconds(1)))		// almost immediate timeout
	{
		if (m_listenerSelect.isReady(m_listener))
		{
			printf("New connection detected\n");
			if (m_listener.accept(m_tcpSocket) != Socket::Done)
			{
				printf("NetworkManager::ReceiveConnections - Connection not accepted\n");
				return;
			}
			m_selector.add(m_tcpSocket);

			Packet packet;
			packet << WELCOME_STRING << ++m_nClients;
			m_tcpSocket.send(packet);
			printf("New connection added\n");
		}
	}
}


void NetworkManager::DealWithPacket(Packet& packet)
{
	unsigned char sender;
	unsigned char type;
	unsigned int packetID;

	packet >> sender >> type >> packetID;

	if (sender == m_networkID)
	{
		printf("NetworkManager::DealWithPacket - this packet came from myself\n");
	}

	switch (type)
	{
		case EntityUpdate:
		{
			UpdateMessage msg;
			packet >> msg;
			m_pPlayers[sender]->TakeMessage(msg);
			break;
		}
		case Gunshot:
		{
			ShotMessage msg;
			packet >> msg;
			m_pPlayers[sender]->YouShot(msg);
			if (msg.playerHit >= 0 && msg.playerHit < m_pPlayers.size())
			{
				m_pPlayers[msg.playerHit]->YouWereShot(msg.damage);
			}
			break;
		}
		case TimeRequest:
		{
			SendTimeReply();
			break;
		}
		case TimeReply:
		{
			float hostTime;
			packet >> hostTime;
			float myTimeNow = Timer::t();
			float roundTripTime = myTimeNow - m_timeOfLastPing;
			if (roundTripTime < m_pingTimeout)
			{
				Timer::ChangeOffset(hostTime - myTimeNow + roundTripTime / 2.f);
			}
			break;
		}
	}
}


Packet NetworkManager::CreateStandardPacket(unsigned char type)
{
	Packet result;
	result << m_networkID << type << m_lastUsedPacketID++;
	return result;
}


void NetworkManager::SendUpdateMessage(const UpdateMessage& msg)
{
	Packet outgoing = CreateStandardPacket(EntityUpdate);
	outgoing << msg;
	m_udpConnection.Send(outgoing);
}


void NetworkManager::SendTimeRequest()
{
	Packet outgoing = CreateStandardPacket(TimeRequest);
	m_timeOfLastPing = Timer::t();
	m_udpConnection.Send(outgoing);
}


void NetworkManager::SendTimeReply()
{
	Packet outgoing = CreateStandardPacket(TimeReply);
	outgoing << Timer::t();
	m_udpConnection.Send(outgoing);
}


void NetworkManager::SendShotMessage(const ShotMessage& msg)
{
	Packet outgoing = CreateStandardPacket(Gunshot);
	outgoing << msg;
	m_tcpSocket.send(outgoing);
}

