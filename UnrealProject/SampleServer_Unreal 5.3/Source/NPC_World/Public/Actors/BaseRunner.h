// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Public/Actors/BaseGun.h"
#include "../../Public/Manager/JellyManager.h"
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
	void DestroyGun();
	void Fire();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void EquipGun(ABaseGun* newGun);
	ABaseGun* GetGun();
	void Attack();
	void CallAimAnimEvent();
	void CallStopAimAnimEvent();
	
	UFUNCTION(BlueprintCallable)
	void Fire(FVector CameraLocation, FRotator CameraRotation, 
		float distance, UParticleSystem* ExplosionEffect, UParticleSystem* StunEffect, UParticleSystem* InkEffect,
		FVector ParticleScale);
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(UAnimMontage* AttackMontage, FName StartSectionName);
	UFUNCTION(BlueprintCallable)
	void PlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName);
	UFUNCTION(BlueprintCallable)
	void StopPlayAimAnimation(UAnimMontage* AimMontage, FName StartSectionName);

	UPROPERTY(BlueprintReadWrite)
	ABaseGun* m_gun;
private:
	bool aiming;
	bool bshoot;
	AJellyManager* JellyManager;
};
