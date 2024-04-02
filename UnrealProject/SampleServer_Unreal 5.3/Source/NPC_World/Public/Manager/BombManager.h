// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <map>
#include <concurrent_queue.h>

#include "../../Public/Actors/Bomb.h"
#include "GameFramework/Actor.h"
#include "../../../../../../Server/ServerTest/ServerTest/protocol.h"
#include "../NetworkingThread.h"

#include "Manager/MyGameInstance.h"
#include "BombManager.generated.h"

UCLASS()
class NPC_WORLD_API ABombManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABombManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void AddBomb(ABomb* newBomb, int idx);
	void ExplosionBomb(int idx);
	void SetBombExplosionQueue(SC_BOMB_EXPLOSION_PACKET* packet);
private:
	std::map<int, ABomb*> Bombs;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
	concurrency::concurrent_queue <SC_BOMB_EXPLOSION_PACKET> Bomb_Explosion_queue;

};
