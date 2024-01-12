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

	void send_packet(void* packet, unsigned id);
	void process_packet(unsigned char* packet, int c_id);
	void do_read();
	void do_write(unsigned char* packet, std::size_t length);
	
public:
	cSession(tcp::socket _socket, int _new_id) : socket(std::move(_socket)), my_id(_new_id)
	{
		curr_packet_size = 0;
		prev_data_size = 0;
	}

	// ========
	void set_user_name(std::string _user_name);

	// ========

	std::string get_user_name();
	int get_my_id();

	// ========
	void start();
	void send_packet(void* packet);
	void send_login_fail_packet();
	void send_login_info_packet();
	
};