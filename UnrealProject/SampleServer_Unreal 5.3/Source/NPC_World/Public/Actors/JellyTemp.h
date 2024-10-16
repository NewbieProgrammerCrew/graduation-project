// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JellyTemp.generated.h"

UCLASS()
class NPC_WORLD_API AJelly : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJelly();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetIndex(int idx) { index = idx;  }
	int GetIndex() const;
	void ExplosionEffect();
	int index;
};
