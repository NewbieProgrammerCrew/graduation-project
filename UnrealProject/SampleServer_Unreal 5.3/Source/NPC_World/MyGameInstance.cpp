// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"
#include "NetworkingThread.h"
#include <future>
#include <mutex>
#include <Kismet/GameplayStatics.h>


FRunnableThread* NetworkThread;

UMyGameInstance::UMyGameInstance()
{ 
	m_playerInfo = new PlayerInfo(); 
	signupSuccess = false;
	loginSuccess = false;
	signUpPacket_Arrived = false;
	loginPacket_Arrived = false;
}

void UMyGameInstance::SetRole(FString role) 
{
	const TCHAR* ch = *role;
	std::wstring ws{ ch };
	m_playerInfo->SetRole(std::string(ws.begin(), ws.end()));
	SendRolePacket();
}
void UMyGameInstance::SetName(FString name)
{
	m_playerInfo->SetName(name);
}
FText UMyGameInstance::GetName()
{
	return FText(FText::FromString(m_playerInfo->GetName()));
}
bool UMyGameInstance::GetSignUpPacketArrivedResult()
{
	return signUpPacket_Arrived;
}
bool UMyGameInstance::GetLoginPacketArrivedResult()
{
	return loginPacket_Arrived;
}
void UMyGameInstance::SetMapId(int id)
{
	mapid = id;
	TWeakObjectPtr<UMyGameInstance> WeakThis = this;
	AsyncTask(ENamedThreads::GameThread, [WeakThis, id]()
		{
			if (WeakThis.IsValid())
			{
				FName IntAsName = FName(*FString::FromInt(id));
				UGameplayStatics::OpenLevel(WeakThis.Get(), IntAsName, true);
			}
		});
}

void UMyGameInstance::SetItemPatternId(int id)
{
	item_pattern = id;
}

void UMyGameInstance::SendMapLoadedPacket()
{

	if (Network->s_socket) {
		CS_MAP_LOADED_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_MAP_LOADED;

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
void UMyGameInstance::SendSignUpPacket(FString id, FString pwd, FString name)
{
	if (Network->s_socket) {
		signUpPacket_Arrived = false;
		CS_SIGNUP_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_SIGNUP;
		strcpy(packet.id,TCHAR_TO_UTF8(*id));
		strcpy(packet.password, TCHAR_TO_UTF8(*pwd));
		strcpy(packet.userName, TCHAR_TO_UTF8(*name));
		
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
void UMyGameInstance::SendLogInPacket(FString id, FString pwd)
{
	if (Network->s_socket) {
		loginPacket_Arrived = false;
		CS_LOGIN_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_LOGIN;
		strcpy(packet.id, TCHAR_TO_UTF8(*id));
		strcpy(packet.password, TCHAR_TO_UTF8(*pwd));

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

void UMyGameInstance::SendRolePacket()
{
	if (Network->s_socket) {
		CS_ROLE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_ROLE;
		strcpy(packet.role, m_playerInfo->GetRole().c_str());
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


bool UMyGameInstance::GetSignUpResult()
{
	return signupSuccess;
}

bool UMyGameInstance::GetLoginResult()
{
	return loginSuccess;
}

int UMyGameInstance::GetErrorLog()
{
	return errorCode;
}

int UMyGameInstance::GetMapId()
{
	return mapid;
}
int UMyGameInstance::GetItemPatternId()
{
	return item_pattern;
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
	const TCHAR* TCHARString = L"192.168.0.27";
	int32 TCHARLength = FCString::Strlen(TCHARString);
	int32 BufferSize = WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, nullptr, 0, nullptr, nullptr);
	ZeroMemory(&Network->IPAddress, 20);
	WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, Network->IPAddress, BufferSize, nullptr, nullptr);
	NetworkThread = FRunnableThread::Create(Network, TEXT("MyThread"), 0, TPri_BelowNormal);
}

void UMyGameInstance::SetUserID()
{
	m_userid = m_temp_id;
}
void UMyGameInstance::SetUserPwd()
{
	m_userpwd = m_temp_pwd;
}
void UMyGameInstance::SetSignupResult(bool result)
{
	signupSuccess = result;
}
void UMyGameInstance::SetLoginResult(bool result)
{
	loginSuccess = result;
}
void UMyGameInstance::SetErrorCode(int error)
{
	errorCode = error;
}
void UMyGameInstance::SetSignUpPacketArrivedResult(bool result)
{
	signUpPacket_Arrived = result;
}
void UMyGameInstance::SetLoginPacketArrivedResult(bool result)
{
	loginPacket_Arrived = result;
}
void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	if (Network) Network->Stop();
}

