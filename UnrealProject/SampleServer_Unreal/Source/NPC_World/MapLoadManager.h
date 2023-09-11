// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapLoadManager.generated.h"

UCLASS()
class NPC_WORLD_API AMapLoadManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapLoadManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FString SelectFilePath;

	void ParseMapFile(const FString& FilePath);
	void SpawnObjectsInWorld(UWorld* World);

	UFUNCTION(BlueprintCallable)
	void OpenFilePicker();
};
