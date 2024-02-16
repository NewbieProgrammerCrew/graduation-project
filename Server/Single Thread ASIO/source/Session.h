#pragma once
#include "Global.h"
#include "IngameData.h"
#include "Protocol.h"

class cSession;

extern concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;

class cSession : public std::enable_shared_from_this<cSession>
{
private:
	tcp::socket		_socket;
	int				_my_id;
	std::string		_user_name;
	unsigned char	_data[BUF_SIZE];
	unsigned char	_packet[BUF_SIZE];
	int				_curr_packet_size;
	int				_prev_data_size;
	int				_ingame_num;

	void Send_Packet(void* packet, unsigned id);
	void Process_Packet(unsigned char* packet, int c_id);
	void Do_Read();
	void Do_Write(unsigned char* packet, std::size_t length);

public:
	bool			_in_use;

public:
	cSession(tcp::socket socket, int new_id) : _socket(std::move(socket)), _my_id(new_id)
	{
		_curr_packet_size = 0;
		_prev_data_size = 0;
		memset(_data, 0, BUF_SIZE);
		memset(_packet, 0, BUF_SIZE);
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
	int	Get_Ingame_Num();
	void Set_Ingame_Num(int num);
};