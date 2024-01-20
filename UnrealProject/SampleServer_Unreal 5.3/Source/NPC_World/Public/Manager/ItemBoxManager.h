// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/ItemBox.h"

#include "../NetworkingThread.h"
#include "Manager/MyGameInstance.h"

#include "ItemBoxManager.generated.h"

UCLASS()
class NPC_WORLD_API AItemBoxManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBoxManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void OpenItemBox(int idx, int gun_id);
	void ClosedItemBox(int idx);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemBox")
	TArray<AItemBox*> ItemBoxes;
	UMyGameInstance* GameInstance = nullptr;
	FSocketThread* Network;
};
