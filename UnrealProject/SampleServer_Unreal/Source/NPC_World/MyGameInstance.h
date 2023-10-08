// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInfo.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class NPC_WORLD_API UMyGameInstance : public UGameInstance {
  GENERATED_BODY()
 public:
  UMyGameInstance();

  UFUNCTION(BlueprintCallable)
  void SetRole(FString role);
  std::string GetRole();
  FString GetRoleF();
 private:
  PlayerInfo* m_playerInfo;
};
