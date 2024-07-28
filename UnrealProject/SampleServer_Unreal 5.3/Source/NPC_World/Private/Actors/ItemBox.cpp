// Fill out your copyright notice in the Description page of Project Settings.
#include "Actors/ItemBox.h"

// Sets default values
AItemBox::AItemBox()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ItemBoxRoot"));

	BombBody = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombBody"));
	bombLeg = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bombLeg"));
	bombLeg1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bombLeg1"));
	BombBody->SetupAttachment(RootComponent);
	bombLeg->SetupAttachment(BombBody);
	bombLeg1->SetupAttachment(BombBody);
}

// Called when the game starts or when spawned
void AItemBox::BeginPlay()
{
	Super::BeginPlay();
	hasBomb = false;
	ChangeBombTypeColorEvent = FindFunction("ChangeBombTypeColor");
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
	if (hasBomb) {
		m_Bombtype = Bombtype;

		if (ChangeBombTypeColorEvent) {
			ProcessEvent(ChangeBombTypeColorEvent, nullptr);
		}
		ShowBombItem();
	}
}

int AItemBox::GetBombItem()
{
	return m_Bombtype;
}

void AItemBox::HideBombItem()
{
	hasBomb = false;
	m_Bombtype = 3;
	BombBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BombBody->SetVisibility(false);
	bombLeg1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bombLeg1->SetVisibility(false);
	bombLeg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bombLeg->SetVisibility(false);
}
void AItemBox::ShowBombItem()
{
	if (hasBomb == false) {
		hasBomb = true;
		BombBody->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BombBody->SetVisibility(true);
	}
}
void AItemBox::SetBoxStatus(bool boxOpen)
{
	Openend = boxOpen;
}
void AItemBox::GetBoxStatus(bool& boxOpen)
{
	boxOpen = Openend;
}


