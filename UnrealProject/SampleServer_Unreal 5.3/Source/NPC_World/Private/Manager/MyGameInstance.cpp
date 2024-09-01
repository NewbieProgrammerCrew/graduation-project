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
	Mutex = new FCriticalSection();

	/*
	currentdebugging = false;
	signupSuccess = false;
	loginSuccess = false;
	signUpPacket_Arrived = false;
	loginPacket_Arrived = false;
	*/
	
	
	
}


void UMyGameInstance::InitializeManagersInNetworkThread()
{
	if (Network) Network->InitializeManagers();
	
	FBoxIdx.Empty();
	FBoxColorId.Empty();


	signupSuccess = false;
	loginSuccess = false;
	signUpPacket_Arrived = false;
	loginPacket_Arrived = false;
	errorCode = 0; 

	//for debugging
	currentdebugging = true;

	mapid = -1;
	characterNum = -1;
	item_pattern = -1;

	m_role = "";


}

void UMyGameInstance::SetRole(int type)
{
	switch (type) {
	case 0:
		m_role = "Runner";
		break;
	case 1:
		m_role = "Chaser";
		break;
	default:
		m_role = "Runner";
		break;
	}
	
}
void UMyGameInstance::SelectCharacter(int ChType)
{
	characterNum = ChType;
}
void UMyGameInstance::SetName(FString name)
{
	m_name = name;
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
	return FText(FText::FromString(m_name));
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
const char* UMyGameInstance::GetRole()
{
	const char* roleAnsi = TCHAR_TO_ANSI(*m_role);
	size_t length = strlen(roleAnsi) + 1;
	char* result = new char[length];
	strcpy_s(result, length, roleAnsi);
	
	return result;
}

FString UMyGameInstance::GetRoleF()
{
	return m_role;
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
	if (Network != nullptr) return;
	Network = new FSocketThread(this);
	//const TCHAR* TCHARString = L"10.30.1.28";
	//const TCHAR* TCHARString = L"127.0.0.1";
	const TCHAR* TCHARString = L"175.198.202.230";

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
	FScopeLock Lock(Mutex);

	if (id == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AddActivedFuseBoxColorId: id is nullptr"));
		return;
	}

	FBoxIdx.Empty();
	for (int i{}; i < 8; ++i) {
		FBoxIdx.Add(id[i]);
	}
}

void UMyGameInstance::AddActivedFuseBoxColorId(int* id)
{
	FScopeLock Lock(Mutex); 
	
	if (id == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AddActivedFuseBoxColorId: id is nullptr"));
		return;
	}

	FBoxColorId.Empty();
	for (int i{}; i < 8; ++i) {
		FBoxColorId.Add(id[i]);
	}
}

void UMyGameInstance::SendMapLoadedPacket()
{
	if (!Network) return;
	if (Network->gs_socket) {
		CS_MAP_LOADED_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_MAP_LOADED;

		WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX( packet.size, &packet);
		if (!wsa_over_ex) {
			return;
		}

		if (WSASend(Network->gs_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_g_callback) == SOCKET_ERROR) {
			int error = WSAGetLastError();
			delete wsa_over_ex;
		}
	}

}
void UMyGameInstance::SendSignUpPacket(FString id, FString pwd, FString name)
{
	if (Network->ls_socket) {
		signUpPacket_Arrived = false;
		CS_SIGNUP_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_SIGNUP;
		
		strcpy_s(packet.id, sizeof(packet.id), TCHAR_TO_UTF8(*id));
		strcpy_s(packet.password, sizeof(packet.password), TCHAR_TO_UTF8(*pwd));
		strcpy_s(packet.userName, sizeof(packet.userName), TCHAR_TO_UTF8(*name));
		
		WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX( packet.size, &packet);
		if (!wsa_over_ex) {
			return;
		}
		if (WSASend(Network->ls_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_g_callback) == SOCKET_ERROR) {
			int error = WSAGetLastError();
			delete wsa_over_ex;
		}
	}
}
void UMyGameInstance::SendLogInPacket(FString id, FString pwd)
{
	if (Network->ls_socket) {
		loginPacket_Arrived = false;
		CS_LOGIN_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_LOGIN;
		strcpy_s(packet.id,sizeof(packet.id), TCHAR_TO_UTF8(*id));
		strcpy_s(packet.password, sizeof(packet.password), TCHAR_TO_UTF8(*pwd));

		WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX( packet.size, &packet);
		if (!wsa_over_ex) {
			return;
		}
		if (WSASend(Network->ls_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_l_callback) == SOCKET_ERROR) {
			int error = WSAGetLastError();
			delete wsa_over_ex;
		}
	}
}

void UMyGameInstance::SendRolePacket()
{
	if (Network->ls_socket) {
		CS_ROLE_PACKET packet;
		packet.size = sizeof(packet);
		packet.type = CS_ROLE;
		strcpy_s(packet.role,sizeof(packet.role), GetRole());
		packet.charactorNum = characterNum;
		WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX( packet.size, &packet);
		if (!wsa_over_ex) {
			return;
		}
		if (WSASend(Network->ls_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_l_callback) == SOCKET_ERROR) {
			int error = WSAGetLastError();
			delete wsa_over_ex;
		}
	}
}
void UMyGameInstance::SendRolePacketToInGame()
{
	if (Network->gs_socket) {
		CS_CONNECT_GAME_SERVER_PACKET pa;
		pa.size = sizeof(pa);
		pa.type = CS_CONNECT_GAME_SERVER;
		
		strcpy_s(pa.role, sizeof(pa.role), GetRole());
		pa.charactorNum = characterNum;
		pa.GroupNum = 0;

		WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(pa.size, &pa);
		if (!wsa_over_ex) {
			return;
		}
		if (WSASend(Network->gs_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_g_callback) == SOCKET_ERROR) {
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


void UMyGameInstance::Shutdown()
{
	if (Network) Network->Exit();
	Super::Shutdown();
}

