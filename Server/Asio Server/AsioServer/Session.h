#pragma once
#include "Global.h"
#include "protocol.h"

class cSession;

extern concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;

class cSession : public std::enable_shared_from_this<cSession>
{
private:
	tcp::socket		socket;
	int				my_id;
	std::string		userName;
	unsigned char	data[BUF_SIZE];
	unsigned char	packet[BUF_SIZE];
	int				curr_packet_size;
	int				prev_data_size;

	void SendPacket(void* packet, unsigned id);
	void Process_Packet(unsigned char* packet, int c_id);
	void do_read();
	void do_write(unsigned char* packet, std::size_t length);
	
public:
	cSession(tcp::socket _socket, int _new_id) : socket(std::move(_socket)), my_id(_new_id)
	{
		curr_packet_size = 0;
		prev_data_size = 0;
	}

	void start();
	
	void SendPacket(void* packet);
	
};