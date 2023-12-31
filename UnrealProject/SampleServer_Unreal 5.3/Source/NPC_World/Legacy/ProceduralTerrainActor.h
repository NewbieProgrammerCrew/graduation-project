// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralTerrainActor.generated.h"

UCLASS()
class NPC_WORLD_API AProceduralTerrainActor : public AActor
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AProceduralTerrainActor();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GenerateVertices(int size);
	UFUNCTION(BlueprintCallable)
	TArray<int> GenerateTriangels(int size);
	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> GenerateUV(int size);
	void UpdateMeshSection(TArray<FVector> vertices);
public:
	TArray<FVector> m_Vertices;
	TArray<int> m_Triangles;
	TArray<FVector2D> m_UV;

private:
	UProceduralMeshComponent* MyMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));

};
