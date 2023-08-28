// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyTCPClient.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameMode();
	virtual void BeginPlay() override;
	virtual void SpawnDefaultPawnFor(AController* NewPlayer, AActor* StartSpot);
	
	UPROPERTY(BlueprintReadWrite)
	UMyTCPClient* TCPClient;
};





