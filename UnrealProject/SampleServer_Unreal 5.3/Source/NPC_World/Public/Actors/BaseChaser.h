// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseChaser.generated.h"

UCLASS()
class NPC_WORLD_API ABaseChaser : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseChaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void UpdateTransform(FRotator rotator, FVector Pos);
	UFUNCTION(BlueprintCallable)
	void GetUpdatedTransform(FRotator& rotator, FVector& Pos);
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Attack();
private:
	FRotator m_rotator{};
	FVector m_pos{};

};
