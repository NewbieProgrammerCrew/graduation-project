// Fill out your copyright notice in the Description page of Project Settings.
#include "Actors/ItemBox.h"

// Sets default values
AItemBox::AItemBox()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AItemBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int AItemBox::GetIndex() const
{
	return idx;
}

void AItemBox::SetGunItem(int guntype)
{
	m_Guntype = guntype;
}

int AItemBox::GetGunItem()
{
	return m_Guntype;
}

