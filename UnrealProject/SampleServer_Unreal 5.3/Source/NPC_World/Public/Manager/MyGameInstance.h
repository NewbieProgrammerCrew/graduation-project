// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <string>
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

struct PlayerInfo 
{
	FString m_name;
	std::string m_role;
};
UCLASS()
class NPC_WORLD_API UMyGameInstance : public UGameInstance 
{
	GENERATED_BODY()
public:
	
	UMyGameInstance();
	
	virtual void Shutdown() override;

	//setter
	UFUNCTION(BlueprintCallable)
	void InitializeManagersInNetworkThread();	
	
	UFUNCTION(BlueprintCallable)
	void SetRole(FString role);
	UFUNCTION(BlueprintCallable)
	void SelectCharacter(int itemType);
	void SetName(FString name);
	void SetMyLobbyID(int id) { lobby_id = id; };
	void SetInGameID(int id) { game_id = id; };
	void SetMapIdAndOpenMap(int id);

	void SetItemPatternId(int id);
	void AddActiveFuseBoxIndex(int* id);
	void AddActivedFuseBoxColorId(int* id);
	void SetNetwork();
	void SetUserID();
	void SetUserPwd();
	void SetSignupResult(bool result);
	void SetLoginResult(bool result);
	void SetErrorCode(int error);
	void SetSignUpPacketArrivedResult(bool result);
	void SetLoginPacketArrivedResult(bool result);
	
	// Getter
	int GetMyLobbyID() { return lobby_id; }
	int GetMyGameID() { return game_id; }
	UFUNCTION(BlueprintCallable)
	int GetMapId();
	UFUNCTION(BlueprintCallable)
	int GetItemPatternId();
	UFUNCTION(BlueprintCallable)
	bool GetSignUpResult();
	UFUNCTION(BlueprintCallable)
	bool GetLoginResult();
	UFUNCTION(BlueprintCallable)
	bool GetSignUpPacketArrivedResult();
	UFUNCTION(BlueprintCallable)
	bool GetLoginPacketArrivedResult();
	UFUNCTION(BlueprintCallable)
	int GetErrorLog();
	FText GetName();
	std::string GetRole();
	UFUNCTION(BlueprintCallable)
	FString GetRoleF();
	int GetCharacterNumber() { return characterNum; }
	TArray<int> GetActiveFuseBoxIndex();
	TArray<int> GetActivedFuseBoxColorId();

	//send packet
	UFUNCTION(BlueprintCallable)
	void SendMapLoadedPacket();
	UFUNCTION(BlueprintCallable)
	void SendSignUpPacket(FString id, FString pwd, FString name);
	UFUNCTION(BlueprintCallable)
	void SendLogInPacket(FString id, FString pwd);
	UFUNCTION(BlueprintCallable)
	void SendRolePacket();
	UFUNCTION(BlueprintCallable)
	bool IsCurrentlyInDebugMode();

	class FSocketThread* Network;

	void DisableLoginSignupForDebug();


	UFUNCTION(BlueprintCallable)
	TArray<int>GetAllInGameCharacterType();
	void AddInGameCharacterInfo(int type);
private:
	TArray<int> Othercharacters;
	//active fusebox
	TArray<int>FBoxIdx;
	TArray<int>FBoxColorId;

	// Success?
	bool signupSuccess;
	bool loginSuccess;
	bool signUpPacket_Arrived;
	bool loginPacket_Arrived;

	int errorCode = 0;
	
	//for debugging
	bool currentdebugging;
	
	int mapid;
	int lobby_id;
	int game_id;
	int characterNum;
	int item_pattern;
	
	PlayerInfo m_playerInfo;
	
	std::string m_userid;
	std::string m_userpwd;
	std::string m_temp_id;
	std::string m_temp_pwd;

};
