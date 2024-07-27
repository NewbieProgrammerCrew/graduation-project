// NetworkThread

#include "NetworkingThread.h"
#include <string>

#include "../Public/Manager/Main.h"
#include "../Public/PlayerController/Ch_PlayerController.h"
#include "../Public/Manager/FuseBoxManager.h"
#include "../Public/Manager/FuseManager.h"
#include "../Public/Manager/PortalManager.h"
#include "../Public/Manager/PlayerManager.h"
#include "../Public/Manager/JellyManager.h"
#include "../Public/Manager/MyGameInstance.h"
#include "../Public/Manager/ItemBoxManager.h"

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

WSA_OVER_EX::WSA_OVER_EX(char byte, void* buf)
{
	ZeroMemory(&_wsaover, sizeof(_wsaover));
	_wsabuf.buf = reinterpret_cast<char*>(buf);
	_wsabuf.len = byte;
}

FSocketThread::FSocketThread()
{

}

FSocketThread::FSocketThread(UGameInstance* inGameInstance)
{
	_gameInstance = Cast<UMyGameInstance>(inGameInstance);
}

uint32_t FSocketThread::Run()
{

	fsocket_thread = this;
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (ret != 0) {
		//exit(-1);
	}
	ls_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(7777);
	inet_pton(AF_INET, IPAddress, &server_addr.sin_addr);
	ret = WSAConnect(ls_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);

	if (ret != 0) {
		//	exit(-1);
	}
	UE_LOG(LogTemp, Warning, TEXT("Network Thread connect!!"));
   
	DWORD r_flags = 0;
	ZeroMemory(&l_recv_over_ex, sizeof(l_recv_over_ex));
	l_recv_over_ex._wsabuf.buf = reinterpret_cast<char*>(l_recv_over_ex._buf);
	l_recv_over_ex._wsabuf.len = sizeof(l_recv_over_ex._buf);
	ret = WSARecv(ls_socket, &l_recv_over_ex._wsabuf, 1, 0, &r_flags, &l_recv_over_ex._wsaover, recv_l_callback);

	if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
	}

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Successfully Received in Network Construct")));
	IsRunning = true;

	while (_MainClass == nullptr || _MyController == nullptr) {
		Sleep(10);
	}

	_gameInstance->DisableLoginSignupForDebug();
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

void FSocketThread::InitializeManagers()
{
	_MainClass = nullptr;
	_JellyManager = nullptr;
	_MyController = nullptr;
	_PlayerManager = nullptr;
	_FuseManager = nullptr;
	_FuseBoxManager = nullptr;
	_PortalManager = nullptr;
	_ItemBoxManager = nullptr;
	_BombManager = nullptr;
	IsInGame = false;
	closesocket(gs_socket);
	
}

void FSocketThread::l_processpacket(unsigned char* buf)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("processpacket case is triggered")));
	unsigned char packet_type = buf[1];
	if (IsRunning) {
		switch (packet_type)
		{
		case SC_SIGNUP:
		{
			SC_SIGNUP_PACKET* packet = reinterpret_cast<SC_SIGNUP_PACKET*>(buf);
			_gameInstance->SetSignupResult(packet->success);
			_gameInstance->SetErrorCode(packet->errorCode);
			_gameInstance->SetSignUpPacketArrivedResult(true);
			break;
		}
		case SC_LOGIN_FAIL:
		{
			SC_LOGIN_FAIL_PACKET* packet = reinterpret_cast<SC_LOGIN_FAIL_PACKET*>(buf);
			_gameInstance->SetLoginPacketArrivedResult(true);
			_gameInstance->SetLoginResult(false);
			_gameInstance->SetErrorCode(packet->errorCode);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Login Fail! ")));
			break;
		}
		case SC_LOGIN_INFO:
		{
			SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(buf);
			_gameInstance->SetLoginResult(true);
			_gameInstance->SetErrorCode(0);
			_gameInstance->SetLoginPacketArrivedResult(true);
			_gameInstance->SetName(packet->userName);
			_gameInstance->SetMyLobbyID(packet->id);
			my_lobby_id = packet->id;
			if (_MyController) {
				_MyController->SetLobbyId(my_lobby_id);
			}
			break;
		}
		case SC_GAME_START:
		{
			int gServerPort;
			SC_GAME_START_PACKET* p = reinterpret_cast<SC_GAME_START_PACKET*>(buf);
			gServerPort = p->portNum;
		
			gs_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
		
			if (gs_socket == INVALID_SOCKET) {
			}
			
			SOCKADDR_IN server_addr;
			ZeroMemory(&server_addr, sizeof(server_addr));
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(gServerPort);
			inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
			
			int ret = WSAConnect(gs_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr), 0, 0, 0, 0);
			
			while (_MainClass == nullptr) { Sleep(100); }
			
			CS_CONNECT_GAME_SERVER_PACKET pa;
			pa.size = sizeof(pa);
			pa.type = CS_CONNECT_GAME_SERVER;
			pa.charactorNum = _gameInstance->GetCharacterNumber();
			strcpy_s(pa.role,sizeof(pa.role), _gameInstance->GetRole());


			pa.GroupNum = 0;
			
			WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(pa.size, &pa);
			if (!wsa_over_ex) {
				return;
			}
			
			if (WSASend(gs_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_g_callback) == SOCKET_ERROR) {
				int error = WSAGetLastError();
				delete wsa_over_ex;
			}
			
			DWORD r_flags = 0;
			ZeroMemory(&g_recv_over_ex, sizeof(g_recv_over_ex));
			g_recv_over_ex._wsabuf.buf = reinterpret_cast<char*>(g_recv_over_ex._buf);
			g_recv_over_ex._wsabuf.len = sizeof(g_recv_over_ex._buf);
			ret = WSARecv(gs_socket, &g_recv_over_ex._wsabuf, 1, 0, &r_flags, &g_recv_over_ex._wsaover, recv_g_callback);
			if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			}
			IsInGame = true;
			while (IsInGame) {
				SleepEx(100, true);
			}
			/*if (gs_socket != INVALID_SOCKET) {
				closesocket(gs_socket);
				gs_socket = INVALID_SOCKET;
			}*/
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

void FSocketThread::g_processpacket(unsigned char* buf)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("processpacket case is triggered")));
	unsigned char packet_type = buf[1];
	if (IsInGame) {
		switch (packet_type)
		{
		case SC_MAP_INFO:
		{
			//UE_LOG(LogTemp, Warning, TEXT("SC_MAP_INFO case is triggered"));
			SC_MAP_INFO_PACKET* packet = reinterpret_cast<SC_MAP_INFO_PACKET*>(buf);
			_MainClass->GameInstance->SetMapIdAndOpenMap(packet->mapid);
			_MainClass->GameInstance->SetItemPatternId(packet->patternid);
			_MainClass->GameInstance->AddActiveFuseBoxIndex(packet->fusebox);
			_MainClass->GameInstance->AddActivedFuseBoxColorId(packet->fusebox_color);
			_MainClass->GameInstance->SetInGameID(packet->id);
			my_game_id = packet->id;
			if (_MyController) {
				_MyController->SetGameId(my_game_id);
			}
			break;
		}
		case SC_ADD_PLAYER:
		{
			SC_ADD_PLAYER_PACKET* packet = reinterpret_cast<SC_ADD_PLAYER_PACKET*>(buf);
			if (_PlayerManager) {
				_PlayerManager->SetPlayerQueue(packet);
			}
			break;
		}
		case SC_MOVE_PLAYER:
		{
			SC_MOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Move_Queue(packet);
			break;
		}
		case SC_ATTACK_PLAYER: {
			SC_ATTACK_PLAYER_PACKET* packet = reinterpret_cast<SC_ATTACK_PLAYER_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Attack_Queue(packet);
			break;
		}
		case SC_HITTED: {
			SC_HITTED_PACKET* packet = reinterpret_cast<SC_HITTED_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Hitted_Queue(packet);
			break;
		}
		case SC_CANNON_FIRE: {
			SC_CANNON_FIRE_PACKET* packet = reinterpret_cast<SC_CANNON_FIRE_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_FireCannon_Queue(packet);
			break;
		}
		case SC_PICKUP_FUSE: {
			SC_PICKUP_FUSE_PACKET* packet = reinterpret_cast<SC_PICKUP_FUSE_PACKET*>(buf);

			if (_FuseManager)
				_FuseManager->Set_Fuse_Destroy_Queue(packet);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Fuse_Pickup_Queue(packet);
			break;
		}
		case SC_PICKUP_BOMB: {
			SC_PICKUP_BOMB_PACKET* packet = reinterpret_cast<SC_PICKUP_BOMB_PACKET*>(buf);
			if (_ItemBoxManager) {
				_ItemBoxManager->Set_SwapBomb(packet);
			}
			if (_PlayerManager)
				_PlayerManager->Set_Player_Bomb_Pickup_Queue(packet);
			break;
		}
		case SC_AIM_STATE: {
			SC_AIM_STATE_PACKET* packet = reinterpret_cast<SC_AIM_STATE_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Aiming_Queue(packet);
			break;
		}
		case SC_IDLE_STATE: {
			SC_IDLE_STATE_PACKET* packet = reinterpret_cast<SC_IDLE_STATE_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Idle_Queue(packet);
			break;
		}
		case SC_USE_SKILL: {
			SC_USE_SKILL_PACKET* packet = reinterpret_cast<SC_USE_SKILL_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Use_Skill_Queue(packet);
			break;
		}
		case SC_SKILL_CHOOSED: {
			SC_SKILL_CHOOSED_PACKET* packet = reinterpret_cast<SC_SKILL_CHOOSED_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Student_Player_Choosed_Skill_Queue(packet);
			break;
		}
		case SC_FUSE_BOX_ACTIVE:
		{
			SC_FUSE_BOX_ACTIVE_PACKET* packet = reinterpret_cast<SC_FUSE_BOX_ACTIVE_PACKET*>(buf);
			if (_FuseBoxManager)
				_FuseBoxManager->Set_FuseBox_Active_Queue(packet);
			break;
		}
		case SC_HALF_PORTAL_GAUGE:
		{
			if (_PortalManager)
				_PortalManager->IncreaseGauge(50);
			break;
		}
		case SC_MAX_PORTAL_GAUGE:
		{
			if (_PortalManager)
				_PortalManager->IncreaseGauge(100);
			break;
		}
		case SC_ESCAPE:
		{
			SC_ESCAPE_PACKET* packet = reinterpret_cast<SC_ESCAPE_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Escape_Queue(packet);
			break;
		}
		case SC_CHASER_WIN:
		{
			SC_CHASER_WIN_PACKET* packet = reinterpret_cast<SC_CHASER_WIN_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Chaser_Win_Queue(packet);
			break;
		}
		case SC_REMOVE_JELLY:
		{
			SC_REMOVE_JELLY_PACKET* packet = reinterpret_cast<SC_REMOVE_JELLY_PACKET*>(buf);
			if (_JellyManager){
				_JellyManager->LookAtBomb(FVector(packet->b_x, packet->b_y, packet->b_z), packet->jellyIndex);
				_JellyManager->ExplosionParticleEvent(packet->jellyIndex);
			}
			break;
		}
		case SC_DEAD: 
		{
			SC_DEAD_PACKET* packet = reinterpret_cast<SC_DEAD_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Dead_Queue(packet);
			break;
		}
		case SC_CHASER_RESURRECTION: {
			SC_CHASER_RESURRECTION_PACKET* packet = reinterpret_cast<SC_CHASER_RESURRECTION_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Resurrect_Queue(packet);
			break;
		}
		case SC_REMOVE_PLAYER:
		{
			SC_REMOVE_PLAYER_PACKET* packet = reinterpret_cast<SC_REMOVE_PLAYER_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Remove_Queue(packet);
			break;
		}
		case SC_OPENING_ITEM_BOX:
		{
			SC_OPENING_ITEM_BOX_PACKET* packet = reinterpret_cast<SC_OPENING_ITEM_BOX_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_ItemBoxOpening_Queue(packet);
		
			break;
		}
		case SC_ITEM_BOX_OPENED:
		{
			SC_ITEM_BOX_OPENED_PACKET* packet = reinterpret_cast<SC_ITEM_BOX_OPENED_PACKET*>(buf);
			if (_ItemBoxManager)
				_ItemBoxManager->Set_OpenBox(packet);
			break;
		}
		case SC_OPENING_FUSE_BOX:
		{
			SC_OPENING_FUSE_BOX_PACKET* packet = reinterpret_cast<SC_OPENING_FUSE_BOX_PACKET*>(buf);
			if (_FuseBoxManager)
				_FuseBoxManager->Set_FuseBox_Opening_Queue(packet);
			if (_PlayerManager)
				_PlayerManager->Set_Player_FuseBoxOpening_Queue(packet);
			break;
		}
		//case SC_RESET_FUSE_BOX:
		//{
		//	SC_RESET_FUSE_BOX_PACKET* packet = reinterpret_cast<SC_RESET_FUSE_BOX_PACKET*>(buf);
		//	if (_FuseBoxManager)
		//		_FuseBoxManager->Set_FuseBox_Reset_Queue(packet);
		//	if (_PlayerManager)
		//		_PlayerManager->Set_Player_Reset_FuseBox_Queue(packet);
		//	break;
		//}
		case SC_FUSE_BOX_OPENED:
		{
			SC_FUSE_BOX_OPENED_PACKET* packet = reinterpret_cast<SC_FUSE_BOX_OPENED_PACKET*>(buf);
			if (_FuseBoxManager)
				_FuseBoxManager->Set_FuseBox_Opened_Queue(packet);
			break;
		}
		case SC_STOP_OPENING:
		{
			SC_STOP_OPENING_PACKET* packet = reinterpret_cast<SC_STOP_OPENING_PACKET*>(buf);
			if (_PlayerManager)
				_PlayerManager->Set_Player_Stop_Opening_Queue(packet);
			if (packet->item == 2) {
				if (_FuseBoxManager)
					_FuseBoxManager->Set_Stop_Opening_Queue(packet);
			}
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
	Destroy();
	FRunnable::Stop();
}

void FSocketThread::Exit()
{
	Destroy();
	FRunnable::Exit();
}
void FSocketThread::Destroy()
{
	IsRunning = false;
	IsInGame = false;
	closesocket(gs_socket);
	closesocket(ls_socket);
	WSACleanup();
}
void CALLBACK send_g_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	if (err != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback ERROR")));
		return;
	}
	WSA_OVER_EX* wsa_over_ex = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete  wsa_over_ex;
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback done")));
}

void CALLBACK recv_g_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD flag)
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
			fsocket_thread->g_processpacket(packet_buffer);
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
	WSARecv(fsocket_thread->gs_socket, &fsocket_thread->g_recv_over_ex._wsabuf, 1, 0, &r_flags, &fsocket_thread->g_recv_over_ex._wsaover, recv_g_callback);
} 



void CALLBACK send_l_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED send_over, DWORD flag)
{
	if (err != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("send callback ERROR"))); 
		return;
	}
	WSA_OVER_EX* wsa_over_ex = reinterpret_cast<WSA_OVER_EX*>(send_over);
	delete  wsa_over_ex;
}

void CALLBACK recv_l_callback(DWORD err, DWORD num_byte, LPWSAOVERLAPPED recv_over, DWORD flag)
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
			fsocket_thread->l_processpacket(packet_buffer);
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
	WSARecv(fsocket_thread->ls_socket, &fsocket_thread->l_recv_over_ex._wsabuf, 1, 0, &r_flags, &fsocket_thread->l_recv_over_ex._wsaover, recv_l_callback);
}