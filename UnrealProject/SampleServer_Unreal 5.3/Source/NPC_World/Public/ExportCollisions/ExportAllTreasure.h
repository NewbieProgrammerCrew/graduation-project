// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Actors/ItemBox.h"
#include "ExportAllTreasure.generated.h"

UCLASS()
class NPC_WORLD_API AExportAllTreasure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExportAllTreasure();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	FString GetExportPath() const;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Export")
	TArray<AActor*> ActorsToExport;
};
