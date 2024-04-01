#pragma once
#include "Global.h"
#include "IngameData.h"
#include "IngameMapData.h"
#include "Protocol.h"

class cSession;

extern concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;

class cSession : public std::enable_shared_from_this<cSession>
{
private:
	tcp::socket		socket_;
	int				my_id_;
	std::string		user_name_;
	unsigned char	data_[BUF_SIZE];
	unsigned char	packet_[BUF_SIZE];
	int				curr_packet_size_;
	int				prev_data_size_;



	void SendPacket(void* packet, unsigned id);
	void ProcessPacket(unsigned char* packet, int c_id);
	void DoRead();
	void DoWrite(unsigned char* packet, std::size_t length);

public:
	bool			in_use_;
	char			role_[PROTOCOL_NAME_SIZE];
	int				charactor_num_;						// 1~5 Runner,  6~7 Chaser
	bool			ready_;
	int				room_num_;
	int				ingame_num_;
	int				ingame_;

public:
	cSession(tcp::socket socket, int new_id) : socket_(std::move(socket)), my_id_(new_id)
	{
		in_use_ = true;
		curr_packet_size_ = 0;
		prev_data_size_ = 0;
		memset(data_, 0, BUF_SIZE);
		memset(packet_, 0, BUF_SIZE);
		room_num_ = -1;
		charactor_num_ = -1;
	}

	// ========
	void Start();
	void SendPacket(void* packet);
	void SendLoginFailPacket();
	void SendLoginInfoPacket();
	void SendMapInfoPacket(SC_MAP_INFO_PACKET p);
	void SendMovePacket(int c_id);
	void SendAttackPacket(int c_id);
	void SendOtherPlayerHittedPacket(int c_id, int hp);
	void SendOtherPlayerDeadPacket(int c_id);
	void SendPickupFusePacket(int c_id, int index);
	void SendCannotInteractivePacket();

	void SendItemBoxOpenedPacket(int index, int bomb_type);
	void SendItemBoxOpeningPacket(int c_id, int index, double progress);
	void SendStopOpeningPacket(int c_id, int item, int index, double progress);
	void SendFuseBoxOpeningPacket(int c_id, int index, double progress);
	void SendFuseBoxOpenedPacket(int index);
	void SendFuseBoxActivePacket(int index);
	void SendHalfPortalGaugePacket();
	void SendMaxPortalGaugePacket();
	void SendPickUpBombPacket(int c_id, int bomb_type, int item_box_index, int left_bomb_type);
	void SendAimStatePacket(int c_id);
	void SendIdleStatePacket(int c_id);

};