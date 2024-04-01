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

void AItemBox::SetBombItem(int Bombtype)
{
	m_Bombtype = Bombtype;
	if (m_Bombtype < 0 || m_Bombtype >= Bomb.Num()) return;
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	ShowBombItem();
	meshes[2]->SetStaticMesh(Bomb[m_Bombtype]);
	meshes[2]->SetMaterial(0, materialsBomb[m_Bombtype]);
}

int AItemBox::GetBombItem()
{
	return m_Bombtype;
}

void AItemBox::HideBombItem()
{
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	hasBomb = false;
	meshes[2]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshes[2]->SetVisibility(false);
	meshes[3]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshes[3]->SetVisibility(false);
	meshes[4]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshes[4]->SetVisibility(false);
}
void AItemBox::ShowBombItem()
{
	TArray<UStaticMeshComponent*> meshes = GetMeshComponent();
	hasBomb = true;
	meshes[2]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	meshes[2]->SetVisibility(true);
}
void AItemBox::SetBoxStatus(bool boxOpen)
{
	Openend = boxOpen;
}
void AItemBox::GetBoxStatus(bool& boxOpen)
{
	boxOpen = Openend;
}


