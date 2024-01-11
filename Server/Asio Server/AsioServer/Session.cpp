#include "Session.h"

using namespace std;

concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;
concurrency::concurrent_unordered_map<std::string, array<std::string, 2>> UserInfo;

void cSession::SendPacket(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	clients[id]->do_write(buff, packet_size);
}

void cSession::Process_Packet(unsigned char* packet, int c_id)
{
	auto P = clients[c_id];
	switch (packet[1]) {
	/*case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			cout << "id is not equal\n";
			clients[c_id].send_login_fail_packet();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id].send_login_fail_packet();
			break;
		}
		clients[c_id]._userName = UserInfo[p->id][1];
		clients[c_id].send_login_info_packet();
		break;
	}*/
	default: cout << "Invalid Packet From Client [" << c_id << "]\n"; //system("pause"); exit(-1);
	}
	/*for (auto& pl : clients)
		pl.second->Send_Packet(&sp);*/
}

void cSession::do_read()
{
	auto self(shared_from_this());
	socket.async_read_some(boost::asio::buffer(data), [this, self](boost::system::error_code ec, std::size_t length) {
		if (ec)
		{
			if (ec.value() == boost::asio::error::operation_aborted) return;
			cout << "Receive Error on Session[" << my_id << "] ERROR_CODE[" << ec << "]\n";
			clients.unsafe_erase(my_id);
			//AvailableUserIDs.insert(my_id);
			//NowUserNum--;
			return;
		}

		int dataToProcess = static_cast<int>(length);
		unsigned char* buf = data;
		while (0 < dataToProcess) {
			if (0 == curr_packet_size) {
				curr_packet_size = buf[0];
				if (buf[0] > 255) {
					cout << "Invalid Packet Size [ << buf[0] << ]\n";
					exit(-1);
				}
			}
			int needToBuild = curr_packet_size - prev_data_size;
			if (needToBuild <= dataToProcess) {
				// 패킷 조립
				memcpy(packet + prev_data_size, buf, needToBuild);
				Process_Packet(packet, my_id);
				curr_packet_size = 0;
				prev_data_size = 0;
				dataToProcess -= needToBuild;
				buf += needToBuild;
			}
			else {
				memcpy(packet + prev_data_size, buf, dataToProcess);
				prev_data_size += dataToProcess;
				dataToProcess = 0;
				buf += dataToProcess;
			}
		}
		do_read();
		});
}

void cSession::do_write(unsigned char* packet, std::size_t length)
{
	auto self(shared_from_this());
	socket.async_write_some(boost::asio::buffer(packet, length), [this, self, packet, length](boost::system::error_code ec, std::size_t bytes_transferred) {
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
}

void cSession::SendPacket(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	do_write(buff, packet_size);
}
