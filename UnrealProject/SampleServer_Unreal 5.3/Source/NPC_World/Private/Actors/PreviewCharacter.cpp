// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PreviewCharacter.h"

// Sets default values
APreviewCharacter::APreviewCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mySkeletonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	RootComponent = mySkeletonMeshComponent;
}

// Called when the game starts or when spawned
void APreviewCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APreviewCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APreviewCharacter::ApplyRunnerCharacterSkeletonMesh(int characterType)
{
	mySkeletonMeshComponent->SetSkeletalMesh(RunnerSkeleltonMesh);
	SetActorScale3D(FVector(2, 2, 2));
	int idx = characterType - 1;
	if (Materials[idx]) {
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Materials[idx], this);
		if (DynamicMaterialInstance) {
			mySkeletonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
	mySkeletonMeshComponent->PlayAnimation(RunnerIdleAnimation, true);

}

void APreviewCharacter::ApplyChaserCharacterSkeletonMesh(int characterType)
{	
	mySkeletonMeshComponent->SetSkeletalMesh(ChaserSkeleltonMesh);
	SetActorScale3D(FVector(0.4, 0.4, 0.4));

	int filter = 5;
	int idx = characterType + filter - 1;
	if (Materials[idx]) {
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(Materials[idx], this);
		if (DynamicMaterialInstance) {
			mySkeletonMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
	mySkeletonMeshComponent->PlayAnimation(ChaserIdleAnimation, true);

}

