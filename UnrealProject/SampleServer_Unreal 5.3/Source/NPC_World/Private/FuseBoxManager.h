// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../FuseBox.h"
#include "../NetworkingThread.h"
#include "../Main.h"
#include "FuseBoxManager.generated.h"

UCLASS()
class AFuseBoxManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseBoxManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ActiveFuseBox(int idx);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FuseBoxes")
	TArray<AFuseBox*> FuseBoxes;
private:
	FSocketThread* Network;
	AMain* Main;
};
