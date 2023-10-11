// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "NetworkingThread.h"
#include <future>
#include <mutex>
#include <Kismet/GameplayStatics.h>

FRunnableThread* NetworkThread;

UMyGameInstance::UMyGameInstance() { 
	m_playerInfo = new PlayerInfo(); 
	menu = true;
}

void UMyGameInstance::SetRole(FString role) {
  const TCHAR *ch = *role;
  std::wstring ws{ch};
    m_playerInfo->SetRole(std::string(ws.begin(),ws.end()));
  }

void UMyGameInstance::SetMapId(int id)
{
	mapid = id;
}

void UMyGameInstance::SendChangeMapPacket()
{
	if (!menu) {
		if (Network->s_socket) {
			CS_CHANGE_MAP_PACKET packet;
			packet.size = sizeof(packet);
			packet.type = CS_CHANGE_MAP;

			WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
			if (!wsa_over_ex) {
				return;
			}

			if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
				int error = WSAGetLastError();
				delete wsa_over_ex;
			}
		}
	}
}

int UMyGameInstance::GetMapId()
{
	return mapid;
}

std::string UMyGameInstance::GetRole()
{
	return m_playerInfo->GetRole();
}

FString UMyGameInstance::GetRoleF() 
{
	return FString(UTF8_TO_TCHAR(m_playerInfo->GetRole().c_str()));
}

void UMyGameInstance::SetNetwork()
{
	Network = new FSocketThread();
	const TCHAR* TCHARString = L"127.0.0.1";
	int32 TCHARLength = FCString::Strlen(TCHARString);
	int32 BufferSize = WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, nullptr, 0, nullptr, nullptr);
	ZeroMemory(&Network->IPAddress, 20);
	WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, Network->IPAddress, BufferSize, nullptr, nullptr);
	NetworkThread = FRunnableThread::Create(Network, TEXT("MyThread"), 0, TPri_BelowNormal);
}

void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	if (Network) Network->Stop();
}

