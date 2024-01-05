// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseGun.h"

// Sets default values
ABaseGun::ABaseGun(int n) : bullets{n}
{
	PrimaryActorTick.bCanEverTick = true;
}
ABaseGun::ABaseGun() :bullets{ 1 } 
{
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
}
void ABaseGun::Fire()
{
	
}

void ABaseGun::UpdateBulletCount(int n)
{
	bullets = n;
}

int ABaseGun::GetBulletCount()
{
	return bullets;
}

// Called when the game starts or when spawned
void ABaseGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

