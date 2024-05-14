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

}

void ABombManager::AddBomb(ABomb* newBomb, int idx)
{
}

void ABombManager::RemoveBomb(int idx)
{
}
void ABombManager::ExplosionBomb(int idx)
{
}
FVector ABombManager::GetBombLocation(int idx)
{
	return FVector();
}


void ABombManager::SetBombExplosionQueue(SC_REMOVE_JELLY_PACKET* packet)
{

}

