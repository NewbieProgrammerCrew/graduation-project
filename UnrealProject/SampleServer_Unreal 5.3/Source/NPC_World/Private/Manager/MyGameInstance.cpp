// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/MyGameInstance.h"
#include "NetworkingThread.h"
#include <future>
#include <mutex>
#include <random> // for debugging
#include <Kismet/GameplayStatics.h>


std::random_device rd;
std::mt19937 dre(rd());
std::uniform_int_distribution<int> uid(32, 126);
FRunnableThread* NetworkThread;

UMyGameInstance::UMyGameInstance()
{ 
	currentdebugging = true;
	
	/*
	currentdebugging = false;
	signupSuccess = false;
	loginSuccess = false;
	signUpPacket_Arrived = false;
	loginPacket_Arrived = false;
	*/
	
	
	
}


void UMyGameInstance::StopNetwork()
{
	if (Network) Network->Stop();
	Network = nullptr;
}

void UMyGameInstance::SetRole(FString role)
{
	const TCHAR* ch = *role;
	std::wstring ws{ ch };
	m_playerInfo.m_role = std::string(ws.begin(), ws.end());
	
}
void UMyGameInstance::SelectCharacter(int ChType)
{
	characterNum = ChType;
}
void UMyGameInstance::SetName(FString name)
{
	m_playerInfo.m_name = name;
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

void UMyGameInstance::SetMapIdAndOpenMap(int id)
{
	mapid = id;

	AsyncTask(ENamedThreads::GameThread, [this, id]()
		{
			UGameplayStatics::OpenLevel(this, FName(*FString::FromInt(id)));
			UFunction* AddLoadWidgetEvent = FindFunction(FName("AddLoadWidgetEvent"));
			if (AddLoadWidgetEvent) {
				ProcessEvent(AddLoadWidgetEvent, nullptr);
			}

		});
}
FText UMyGameInstance::GetName()
{
	return FText(FText::FromString(m_playerInfo.m_name));
}
bool UMyGameInstance::GetSignUpPacketArrivedResult()
{
	return signUpPacket_Arrived;
}
bool UMyGameInstance::GetLoginPacketArrivedResult()
{
	return loginPacket_Arrived;
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
	return m_playerInfo.m_role;
}
FString UMyGameInstance::GetRoleF()
{
	return FString(UTF8_TO_TCHAR(m_playerInfo.m_role.c_str()));
}
TArray<int> UMyGameInstance::GetActiveFuseBoxIndex()
{
	return FBoxIdx;
}
TArray<int> UMyGameInstance::GetActivedFuseBoxColorId()
{
	return FBoxColorId;
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

void UMyGameInstance::SetItemPatternId(int id)
{
	item_pattern = id;
}

void UMyGameInstance::AddActiveFuseBoxIndex(int* id)
{
	for (int i{}; i < 8; ++i) {
		FBoxIdx.Add(id[i]);
	}
}

void UMyGameInstance::AddActivedFuseBoxColorId(int* id)
{
	for (int i{}; i < 8; ++i) {
		FBoxColorId.Add(id[i]);
	}
}

void UMyGameInstance::SendMapLoadedPacket()
{
	if (!Network) return;
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
		
		strcpy_s(packet.id, sizeof(packet.id), TCHAR_TO_UTF8(*id));
		strcpy_s(packet.password, sizeof(packet.password), TCHAR_TO_UTF8(*pwd));
		strcpy_s(packet.userName, sizeof(packet.userName), TCHAR_TO_UTF8(*name));
		
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
		strcpy_s(packet.id,sizeof(packet.id), TCHAR_TO_UTF8(*id));
		strcpy_s(packet.password, sizeof(packet.password), TCHAR_TO_UTF8(*pwd));

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
		strcpy_s(packet.role,sizeof(packet.role), m_playerInfo.m_role.c_str());
		packet.charactorNum = characterNum;
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

bool UMyGameInstance::IsCurrentlyInDebugMode()
{
	return currentdebugging;
}

void UMyGameInstance::DisableLoginSignupForDebug()
{
	signupSuccess = true;
	loginSuccess = true;
	signUpPacket_Arrived = true;
	loginPacket_Arrived = true;

	auto generate_random_string = [&](int len) -> std::string {
		std::string str;
		str.reserve(len);
		for (int i = 0; i < len - 1; ++i) {
			char random_char = static_cast<char>(uid(dre));
			str += random_char;
		}
		return str + '\0';
	};

	std::string t_id = generate_random_string(10);   
	std::string t_pwd = generate_random_string(10);
	std::string t_name = generate_random_string(10);
	SendSignUpPacket(FString(t_id.c_str()), FString(t_pwd.c_str()), FString(t_name.c_str()));
	SendLogInPacket(FString(t_id.c_str()), FString(t_pwd.c_str()));
}

TArray<int> UMyGameInstance::GetAllInGameCharacterType()
{
	return Othercharacters;
}

void UMyGameInstance::AddInGameCharacterInfo(int type)
{
	if (type > -1) {
		Othercharacters.Add(type);
	}
}


void UMyGameInstance::Shutdown()
{
	Super::Shutdown();

	if (Network) Network->Stop();
}

