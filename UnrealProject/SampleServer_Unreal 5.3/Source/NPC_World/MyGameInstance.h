// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.h"
#include <string>
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"


UCLASS()
class NPC_WORLD_API UMyGameInstance : public UGameInstance 
{
	GENERATED_BODY()
public:
	
	UMyGameInstance();
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	void SetRole(FString role);
	void SetMapId(int id);

	UFUNCTION(BlueprintCallable)
	int GetMapId();

	//send packet
	UFUNCTION(BlueprintCallable)
	void SendChangeMapPacket();
	UFUNCTION(BlueprintCallable)
	void SendSignUpPacket(FString id, FString pwd, FString name);
	UFUNCTION(BlueprintCallable)
	void SendLogInPacket(FString id, FString pwd);
	UFUNCTION(BlueprintCallable)
	bool GetSignUpResult();
	UFUNCTION(BlueprintCallable)
	int GetErrorLog();

	void SetNetwork();
	class FSocketThread* Network;

	std::string GetRole();
	FString GetRoleF();


	UPROPERTY(BlueprintReadWrite)
	bool menu;
	UPROPERTY(BlueprintReadWrite)
	bool getSignUpPacket;
	// Success?
	bool signupSuccess;
	bool loginSuccess;

	int errorCode = 0;
	
	void SetUserID();
	void SetUserPwd();

private:
	PlayerInfo* m_playerInfo;
	int mapid;
	std::string m_userid;
	std::string m_userpwd;

	std::string m_temp_id;
	std::string m_temp_pwd;
};
