// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Manager/MyGameInstance.h"
#include "../../Public/Actors/JellyTemp.h"
#include "../../Public/NetworkingThread.h"
#include "../../../../../Server/ServerTest/ServerTest/protocol.h"

#include "JellyManager.generated.h"

UCLASS()
class NPC_WORLD_API AJellyManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AJellyManager();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	virtual void Tick(float DeltaTime) override;
	void LookAtPlayer(ACharacter* Player, int idx);
	void ExplosionParticleEvent(int idx);
public:
	UPROPERTY(EditAnywhere)
	TArray<AJelly*> jellies;
	FSocketThread* network;
	UMyGameInstance* GameInstance;


};
