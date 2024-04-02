// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/BombManager.h"

// Sets default values
ABombManager::ABombManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABombManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABombManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GameInstance)
		GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (GameInstance) {
		if (!Network && GameInstance->Network) {
			Network = GameInstance->Network;
			if (!GameInstance->Network->_BombManager) {
				GameInstance->Network->_BombManager = this;
			}
		}
	}
	SC_BOMB_EXPLOSION_PACKET explosionBomb;
	while (!Bomb_Explosion_queue.empty()) {
		if (Bomb_Explosion_queue.try_pop(explosionBomb)) {
			int idx = explosionBomb.bomb_index;
			ExplosionBomb(idx);
		}
	}
}

void ABombManager::AddBomb(ABomb* newBomb, int idx)
{
	Bombs.Add(idx, newBomb);
}

void ABombManager::ExplosionBomb(int idx)
{
	if ((Bombs.Contains(idx)) && Bombs[idx]) {
		Bombs[idx]->Destroy();
		Bombs[idx] = nullptr;
	}
}
void ABombManager::SetBombExplosionQueue(SC_BOMB_EXPLOSION_PACKET* packet)
{
	Bomb_Explosion_queue.push(*packet);
}

