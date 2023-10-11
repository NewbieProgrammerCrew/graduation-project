// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.h"
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
	void SendChangeMapPacket();
	UFUNCTION(BlueprintCallable)
	int GetMapId();
	std::string GetRole();
	FString GetRoleF();

	void SetNetwork();
	class FSocketThread* Network;
	UPROPERTY(BlueprintReadWrite)
	bool menu;
private:
	PlayerInfo* m_playerInfo;

	int mapid;
};
