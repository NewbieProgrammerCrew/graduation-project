// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Actors/FuseBox.h"
#include "ExportAllFuseBoxInfo.generated.h"

UCLASS()
class NPC_WORLD_API AExportAllFuseBoxInfo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExportAllFuseBoxInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	void WriteFuseBoxInfoToJson();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TArray<AActor*> ActorsToExport;
	UPROPERTY(EditAnywhere)
	AActor* FuseBoxActor;
	
private:
	FString GetExportPath() const;

};
