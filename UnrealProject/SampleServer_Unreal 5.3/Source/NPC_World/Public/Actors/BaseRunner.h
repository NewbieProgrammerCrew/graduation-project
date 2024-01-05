// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Public/Actors/BaseGun.h"
#include "BaseRunner.generated.h"

UCLASS()
class NPC_WORLD_API ABaseRunner : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseRunner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlayAttackMontage();
	void DestroyGun();
	void Fire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void EquipGun(ABaseGun* newGun);
	ABaseGun* GetGun();
	void Attack();
	void SetAimMode();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(BlueprintReadWrite)
	ABaseGun* m_gun;
private:
	bool aiming;
	bool bshoot;
};
