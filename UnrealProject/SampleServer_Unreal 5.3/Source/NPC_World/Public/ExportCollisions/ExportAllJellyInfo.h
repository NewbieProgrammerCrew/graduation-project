// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExportAllJellyInfo.generated.h"

UCLASS()
class NPC_WORLD_API AExportAllJellyInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExportAllJellyInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override; 
	FString GetExportPath() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision Export")
	TArray<AActor*> ActorsToExport;
};
