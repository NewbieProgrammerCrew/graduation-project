#include "Session.h"

using namespace std;

concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;

void cSession::SendPacket(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	clients[id]->do_write(buff, packet_size);
}

void cSession::Process_Packet(unsigned char* packet, int id)
{
	auto P = clients[id];
	switch (packet[1]) {
	default: cout << "Invalid Packet From Client [" << id << "]\n"; system("pause"); exit(-1);
	}
	/*for (auto& pl : clients)
		pl.second->Send_Packet(&sp);*/
}

void cSession::do_read()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(data),
		[this, self](boost::system::error_code ec, std::size_t length)
		{
			if (ec)
			{
				if (ec.value() == boost::asio::error::operation_aborted) return;
				cout << "Receive Error on Session[" << my_id << "] EC[" << ec << "]\n";
				clients[my_id] == nullptr;
				//clients.unsafe_erase(my_id_);
				return;
			}

			int data_to_process = static_cast<int>(length);
			unsigned char* buf = data;
			while (0 < data_to_process) {
				if (0 == curr_packet_size_) {
					curr_packet_size_ = buf[0];
					if (buf[0] > 200) {
						cout << "Invalid Packet Size [ << buf[0] << ] Terminating Server!\n";
						exit(-1);
					}
				}
				int need_to_build = curr_packet_size_ - prev_data_size_;
				if (need_to_build <= data_to_process) {
					// 패킷 조립
					memcpy(packet + prev_data_size_, buf, need_to_build);
					Process_Packet(packet, my_id);
					curr_packet_size_ = 0;
					prev_data_size_ = 0;
					data_to_process -= need_to_build;
					buf += need_to_build;
				}
				else {
					// 훗날을 기약
					memcpy(packet + prev_data_size_, buf, data_to_process);
					prev_data_size_ += data_to_process;
					data_to_process = 0;
					buf += data_to_process;
				}
			}
			do_read();
		});
}

void cSession::do_write(unsigned char* packet, std::size_t length)
{
	auto self(shared_from_this());
	socket.async_write_some(boost::asio::buffer(packet, length),
		[this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred)
		{
			if (!ec)
			{
				if (length != bytes_transferred) {
					cout << "Incomplete Send occured on Session[" << my_id << "]. This Session should be closed.\n";
				}
				delete packet;
			}
		});
}

void cSession::start()
{
	do_read();
	/*sc_packet_login_info pl;
	pl.id = my_id_;
	pl.size = sizeof(sc_packet_login_info);
	pl.type = SC_LOGIN_INFO;
	pl.x = pos_x;
	pl.y = pos_y;
	Send_Packet(&pl);

	sc_packet_put_player p;
	p.id = my_id_;
	p.size = sizeof(sc_packet_put_player);
	p.type = SC_PUT_PLAYER;
	p.x = pos_x;
	p.y = pos_y;*/

	// 나의 접속을 모든 플레이어에게 알림
	/*for (auto& pl : clients)
		if (pl.second != nullptr)
			pl.second->Send_Packet(&p);*/
			// 나에게 접속해 있는 다른 플레이어 정보를 전송
			// 나에게 주위에 있는 NPC의 정보를 전송
		/*	for (auto& pl : clients) {
				if (pl.second->my_id_ != my_id_) {
					p.id = pl.second->my_id_;
					p.x = pl.second->pos_x;
					p.y = pl.second->pos_y;
					Send_Packet(&p);
				}
			}*/
}

void cSession::SendPacket(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	do_write(buff, packet_size);
}
