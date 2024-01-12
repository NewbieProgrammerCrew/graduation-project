#include "Session.h"

using namespace std;

concurrency::concurrent_unordered_map<int, shared_ptr<cSession>> clients;
concurrency::concurrent_unordered_map<std::string, array<std::string, 2>> UserInfo;
concurrency::concurrent_unordered_set<std::string> UserName;
extern concurrency::concurrent_queue<int> AvailableUserIDs;
extern atomic_int NowUserNum;



void cSession::send_packet(void* packet, unsigned id)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	clients[id]->do_write(buff, packet_size);
}

void cSession::process_packet(unsigned char* packet, int c_id)
{
	auto P = clients[c_id];
	switch (packet[1]) {
	case CS_LOGIN: {
		CS_LOGIN_PACKET* p = reinterpret_cast<CS_LOGIN_PACKET*>(packet);
		if (UserInfo.find(p->id) == UserInfo.end()) {
			cout << "id is not equal\n";
			clients[c_id]->send_login_fail_packet();
			break;
		}
		else if (UserInfo[p->id][0] != p->password) {
			cout << "pwd is not equal\n";
			clients[c_id]->send_login_fail_packet();
			break;
		}
		clients[c_id]->set_user_name(UserInfo[p->id][1]);
		clients[c_id]->send_login_info_packet();
		break;
	}

	case CS_SIGNUP: {
		CS_SIGNUP_PACKET* p = reinterpret_cast<CS_SIGNUP_PACKET*>(packet);
		cout << "id: " << p->id << endl;
		cout << "pwd: " << p->password << endl;
		cout << "name: " << p->userName << endl;
		SC_SIGNUP_PACKET signupPacket;
		signupPacket.type = SC_SIGNUP;
		signupPacket.size = sizeof(SC_SIGNUP_PACKET);

		if (UserInfo.find(p->id) != UserInfo.end()) {	// 중복되는 아이디가 있는지 확인
			signupPacket.success = false;
			signupPacket.errorCode = 100;
			cout << "이미 사용중인 아이디 입니다.\n";
			clients[c_id]->send_packet(&signupPacket);
			break;
		}

		if (UserName.find(p->userName) != UserName.end()) {	// 중복되는 닉네임이 있는지 확인.
			signupPacket.success = false;
			signupPacket.errorCode = 101;
			clients[c_id]->send_packet(&signupPacket);
			break;
		}
		UserName.insert(p->userName);

		UserInfo[p->id] = { p->password, p->userName };
		signupPacket.success = true;
		signupPacket.errorCode = 0;
		clients[c_id]->send_packet(&signupPacket);
		break;
	}
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
			AvailableUserIDs.push(my_id);
			NowUserNum--;
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
				process_packet(packet, my_id);
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

void cSession::set_user_name(std::string _user_name)
{
	user_name = _user_name;
}

std::string cSession::get_user_name()
{
	return user_name;
}

void cSession::start()
{
	do_read();
}

void cSession::send_packet(void* packet)
{
	int packet_size = reinterpret_cast<unsigned char*>(packet)[0];
	unsigned char* buff = new unsigned char[packet_size];
	memcpy(buff, packet, packet_size);
	do_write(buff, packet_size);
}

int cSession::get_my_id()
{
	return my_id;
}

void cSession::send_login_fail_packet()
{
	SC_LOGIN_FAIL_PACKET p;
	p.id = get_my_id();
	p.size = sizeof(SC_LOGIN_FAIL_PACKET);
	p.type = SC_LOGIN_FAIL;
	p.errorCode = 102;
	send_packet(&p);
}

void cSession::send_login_info_packet()
{
	SC_LOGIN_INFO_PACKET p;
	p.id = get_my_id();
	p.size = sizeof(SC_LOGIN_INFO_PACKET);
	p.type = SC_LOGIN_INFO;
	std::string user_name = get_user_name();
	strcpy(p.userName, user_name.c_str());

	send_packet(&p);
}
