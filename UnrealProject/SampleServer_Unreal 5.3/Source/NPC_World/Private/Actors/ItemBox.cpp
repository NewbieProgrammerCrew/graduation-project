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
	if (m_Guntype < 0) return;
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	ShowGunItem();
	meshes[2]->SetStaticMesh(Gun[m_Guntype]);
	meshes[2]->SetMaterial(0, materialsGun[m_Guntype]);
}

int AItemBox::GetGunItem()
{
	return m_Guntype;
}

void AItemBox::HideGunItem()
{
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	m_Guntype = -1;
	meshes[2]->SetVisibility(false);
	meshes[2]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AItemBox::ShowGunItem()
{
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	meshes[2]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	meshes[2]->SetVisibility(true);
}

