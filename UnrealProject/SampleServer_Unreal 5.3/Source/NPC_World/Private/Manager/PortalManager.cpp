// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/PortalManager.h"
#include "Kismet/GameplayStatics.h"
#include "NetworkingThread.h"
// Sets default values
APortalManager::APortalManager()
{
 	PrimaryActorTick.bCanEverTick = true;
	max_Gauge = 100;
	current_Gauge = 0;
}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	if (!world) {
		return;
	}
	AActor* actor = UGameplayStatics::GetActorOfClass(world, APlayerManager::StaticClass());
	if (actor == nullptr) {
		return;
	}
	actor->GetWorld();
	m_playerManager = Cast<APlayerManager>(actor);

	
}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(m_playerManager && m_playerManager->Network && !(m_playerManager->Network->_PortalManager))
		m_playerManager->Network->_PortalManager = this;

}

void APortalManager::IncreaseGauge(int percent)
{
	current_Gauge = percent;
	m_playerManager->PortalGagueUpdate(CalculatePortalPercentRatio());
}

void APortalManager::OpenPortal()
{
	if (CalculatePortalPercentRatio() >= 0.99999f) {
		for (AActor* p : Portals) {
			//ÆÄÆ¼Å¬ visible(true)
		}
	}
}

float APortalManager::CalculatePortalPercentRatio()
{
	return current_Gauge/max_Gauge;
}

