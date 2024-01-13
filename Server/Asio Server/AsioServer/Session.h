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
	std::string		user_name;
	unsigned char	data[BUF_SIZE];
	unsigned char	packet[BUF_SIZE];
	int				curr_packet_size;
	int				prev_data_size;

	void Send_Packet(void* packet, unsigned id);
	void Process_Packet(unsigned char* packet, int c_id);
	void Do_Read();
	void Do_Write(unsigned char* packet, std::size_t length);
	
public:
	cSession(tcp::socket _socket, int _new_id) : socket(std::move(_socket)), my_id(_new_id)
	{
		curr_packet_size = 0;
		prev_data_size = 0;
	}

	// ========
	void Set_User_Name(std::string _user_name);

	// ========

	std::string Get_User_Name();
	int Get_My_Id();

	// ========
	void Start();
	void Send_Packet(void* packet);
	void Send_Login_Fail_Packet();
	void Send_Login_Info_Packet();
	
};