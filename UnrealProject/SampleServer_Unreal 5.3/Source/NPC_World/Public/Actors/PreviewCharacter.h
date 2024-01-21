// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "PreviewCharacter.generated.h"

UCLASS()
class NPC_WORLD_API APreviewCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APreviewCharacter();
	UPROPERTY(EditAnywhere)
	USkeletalMesh* ChaserSkeleltonMesh;
	UPROPERTY(EditAnywhere)
	USkeletalMesh* RunnerSkeleltonMesh;
	USkeletalMeshComponent* mySkeletonMeshComponent;
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInterface*> Materials{};
	UPROPERTY(EditAnywhere)
	UAnimationAsset* ChaserIdleAnimation;
	UPROPERTY(EditAnywhere)
	UAnimationAsset* RunnerIdleAnimation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ApplyChaserCharacterSkeletonMesh(int characterType);
	void ApplyRunnerCharacterSkeletonMesh(int characterType);
	
};
