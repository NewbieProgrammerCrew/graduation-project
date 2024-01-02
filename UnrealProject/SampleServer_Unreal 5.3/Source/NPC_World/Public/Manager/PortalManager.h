// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Manager/PlayerManager.h"
#include "PortalManager.generated.h"

UCLASS()
class NPC_WORLD_API APortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	APortalManager();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
public:
	void IncreaseGauge(int percent);
	float CalculatePortalPercentRatio();
	void OpenPortal();
public:	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<AActor*> Portals;
private:
	float max_Gauge;
	float current_Gauge;
	APlayerManager* m_playerManager;
};
