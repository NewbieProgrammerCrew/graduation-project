// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FuseBox.generated.h"

UCLASS()
class NPC_WORLD_API AFuseBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFuseBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	int GetIndex() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ExportInfo")
	int index;

};
