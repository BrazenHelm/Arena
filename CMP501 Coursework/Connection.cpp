#include "Connection.h"

using namespace sf;


Connection::Connection(UdpSocket* pSocket, std::string ip, unsigned short port) :
	m_pSocket(pSocket), m_ip(ip), m_port(port)
{
}


Connection::~Connection()
{
}


void Connection::Send(Packet packet)
{
	m_pSocket->send(packet, m_ip, m_port);
}