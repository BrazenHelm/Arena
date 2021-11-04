#include "UdpConnection.h"

using namespace sf;

UdpConnection::UdpConnection()
{
}


UdpConnection::~UdpConnection()
{
}


void UdpConnection::Initialise(UdpSocket* pSocket, std::string ip, unsigned short port)
{
	m_pSocket = pSocket;
	m_ip = ip;
	m_port = port;

	m_init = true;
}


void UdpConnection::Initialise(UdpSocket* pSocket, Packet address)
{
	m_pSocket = pSocket;
	address >> m_ip >> m_port;

	m_init = true;
}


void UdpConnection::Send(Packet packet)
{
	if (!m_init)
	{
		printf("UdpConnection::Send - Connection not initialised\n");
		return;
	}
	m_pSocket->send(packet, m_ip, m_port);
}


void UdpConnection::Receive(Packet& packet)
{
	if (!m_init)
	{
		printf("UdpConnection::Receive - Connection not initialised\n");
		return;
	}
	IpAddress remoteAddress;
	unsigned short remotePort;
	m_pSocket->receive(packet, remoteAddress, remotePort);

	if (remoteAddress.toString() != m_ip || remotePort != m_port)
	{
		printf("UdpConnection::Receive - Message from unexpected source\n");
		return;
	}
}

