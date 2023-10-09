// NetworkThread

#include "NetworkingThread.h"
#include "Main.h"
#include <string>
#include "MyPlayerController.h"
#include "PlayerManager.h"

using namespace std;
FSocketThread* fsocket_thread;
NetworkingThread::NetworkingThread()
{ }

NetworkingThread::~NetworkingThread()
{

}

WSA_OVER_EX::WSA_OVER_EX()
{
	//exit(-1);
	return;
}

WSA_OVER_EX::WSA_OVER_EX(IOCPOP iocpop, char byte, void* buf)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_iocpop = iocpop;
	_wsabuf.buf = reinterpret_cast<char*>(buf);
	_wsabuf.len = byte;
}

FSocketThread::FSocketThread()
{

}

uint32_t FSocketThread::Run()
{

	fsocket_thread = this;
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (ret != 0) {
		//exit(-1);
	}
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	inet_pton(AF_INET, IPAddress, &server_addr.sin_addr);
	ret = WSAConnect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);
	if (ret != 0) {
		//	exit(-1);
	}
	UE_LOG(LogTemp, Warning, TEXT("Network Thread connect!!"));
   
	DWORD r_flags = 0;
	ZeroMemory(&_recv_over_ex, sizeof(_recv_over_ex));
	_recv_over_ex._wsabuf.buf = reinterpret_cast<char*>(_recv_over_ex._buf);
	_recv_over_ex._wsabuf.len = sizeof(_recv_over_ex._buf);
	ret = WSARecv(s_socket, &_recv_over_ex._wsabuf, 1, 0, &r_flags, &_recv_over_ex._wsaover, recv_callback);
	if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Recv Error in Network Construct")));
	}
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Successfully Received in Network Construct")));
	IsRunning = true;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("finished construct network")));
	while (_MainClass == nullptr || _MyController == nullptr || _PlayerManager == nullptr) {
		Sleep(10);
	}

	
	CS_LOGIN_PACKET packet_login;
	packet_login.size = sizeof(packet_login);
	packet_login.type = CS_LOGIN;
    string role = _MainClass->GameInstance->GetRole();
    strcpy(packet_login.role, role.c_str());

	WSA_OVER_EX* wsa_over_ex = new WSA_OVER_EX(IOCPOP::OP_SEND, sizeof(CS_LOGIN_PACKET), &packet_login);
	ret = WSASend(s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback);
	
	//여기에 보내면???
	int iter = 0;
	while (IsRunning) {
		SleepEx(100, true);
	}
	UE_LOG(LogTemp, Warning, TEXT("Network Thread End!!"));
	return 0;
}

void FSocketThread::error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);

	LocalFree(lpMsgBuf);
}

void FSocketThread::processpacket(unsigned char* buf)
{
	unsigned char packet_type = buf[1];
	if (IsRunning) {
		switch (packet_type)
		{
		case SC_LOGIN_INFO:
		{
			SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(buf);
			_MyController->id = my_id = packet->id;
			break;
		}

		case SC_ADD_PLAYER:
		{
			UE_LOG(LogTemp, Warning, TEXT("SC_ADD_PLAYER case is triggered"));

			SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(buf);
			_PlayerManager->SetPlayerQueue(packet);

			break;
		}
		case SC_MOVE_PLAYER:
		{
			SC_MOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(buf);
			_PlayerManager->Set_Player_Move_Queue(packet);
			break;
		}
        case SC_ATTACK_PLAYER: {
            SC_ATTACK_PLAYER_PACKET* packet = reinterpret_cast<SC_ATTACK_PLAYER_PACKET*>(buf);
            _PlayerManager->Set_Player_Attack_Queue(packet);
            break;
        }
		case SC_HITTED: {
			SC_HITTED_PACKET* packet = reinterpret_cast<SC_HITTED_PACKET*>(buf);
			_PlayerManager->Set_Player_Hitted_Queue(packet);
			break;
		}
		case SC_DEAD: {
			SC_DEAD_PACKET* packet = reinterpret_cast<SC_DEAD_PACKET*>(buf);
			_PlayerManager->Set_Player_Dead_Queue(packet);
			break;
		}
		case SC_REMOVE_PLAYER:
		{
			SC_REMOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(buf);
			_PlayerManager->Set_Player_Remove_Queue(packet);
			break;
		}

		default:
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("UNKNOWN Packet Type: %d"), (int)packet_type));
			break;
		}
		}
	}
}

void FSocketThread::Stop()
{
	IsRunning = false;
}

void CALLBACK send_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback start")));
	if (err != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback ERROR")));
		return;
	}
	WSA_OVER_EX* wsa_over_ex = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete  wsa_over_ex;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback done")));
}

void CALLBACK recv_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD flag)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("recv callback start")));
	if (err != 0) {
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("recv callback ERROR")));
		return;
	}
	WSA_OVER_EX* wsa_over_ex = reinterpret_cast<WSA_OVER_EX*>(recv_over);

	//패킷 재조립.
	unsigned char* packet_start = wsa_over_ex->_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static unsigned char packet_buffer[BUF_SIZE];

	while (0 != num_byte) {
		if (0 == in_packet_size) in_packet_size = packet_start[0];
		if (num_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, packet_start, in_packet_size - saved_packet_size);
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("recv callback num bytes: %d"), in_packet_size));
			fsocket_thread->processpacket(packet_buffer);
			packet_start += in_packet_size - saved_packet_size;
			num_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, packet_start, num_byte);
			saved_packet_size += num_byte;
			num_byte = 0;
		}
	}

	ZeroMemory(&wsa_over_ex->_wsaover, sizeof(wsa_over_ex->_wsaover));
	DWORD r_flags = 0;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("recv callback done")));
	WSARecv(fsocket_thread->s_socket, &fsocket_thread->_recv_over_ex._wsabuf, 1, 0, &r_flags, &fsocket_thread->_recv_over_ex._wsaover, recv_callback);
} 