// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Public/Manager/JellyManager.h"
#include "../../Public/Actors/ItemBox.h"
#include "../../Public/Actors/FuseBox.h"
#include "../../Public/Actors/Bomb.h"
#include "PlayerComponents/DataUpdater.h"
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
	void DestroyBomb();
	void Fire();
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
    UChildActorComponent* BombChildActorComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* BombMontage;
	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void Attack();
	void PlayAimAnim();
	void StopAimEvent();
	void EquipBomb(ABomb* newBomb);
	ABomb* GetBomb();
	void CallBoxOpenAnimEvent();
	void CallFuseBoxOpenAnimEvent();

	void StartFillingProgressBar();
	void SetOpenItemBoxStartPoint(float startpoint);
	void FillProgressBar();
	UFUNCTION(BlueprintCallable)
	void StopFillingProgressBar();

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
	
	UFUNCTION(BlueprintCallable)
	void SetOpeningBox(bool openingbox);
	UFUNCTION(BlueprintCallable)
	void GetOpeningBox(bool& openingbox);
	UFUNCTION(BlueprintCallable)
	void SetOpeningFuseBox(bool openingbox);
	UFUNCTION(BlueprintCallable)
	void GetOpeningFuseBox(bool& openingbox);
	UFUNCTION(BlueprintCallable)
	void SetCurrentItemBox(AItemBox* itembox);
	UFUNCTION(BlueprintCallable)
	float GetCurrentOpeningItemBoxProgress();
	UFUNCTION(BlueprintCallable)
	bool checkItemBoxAvailable();
	UFUNCTION(BlueprintCallable)
	bool FindItemBoxAndCheckEquipableBomb(FVector CameraLocation, FRotator CameraRotation, float distance);
	UFUNCTION(BlueprintCallable)
	bool FindFuseBoxInViewAndCheckPutFuse(AFuseBox* HitFuseBox);

	FHitResult PerformLineTrace(FVector CameraLocation, FRotator CameraRotation, float distance);
	void ClearOpeningBoxData();
	bool UpdateEquipableBombData(FHitResult Hit, AItemBox* itemBox, UDataUpdater* dataUpdater);
	bool IsFacingFuseBox(AFuseBox* FacingFuseBox);
	void ProcessCustomEvent(AActor* actor, FName Name);
	void StopInteraction();
	void CallDestroyBombbyTimer();

	void PlayMontage(UAnimMontage* MontageToPlay, FName startSection = "Default");
	void StopMontage(UAnimMontage* MontageToStop, FName startSection = "None");

private:

	bool bOpeningBox{};
	bool bOpeningFuseBox{};
	float startPoint{};
	float CurrentProgressBarValue{};
	bool aiming{};
	bool bshoot{};
	FTimerHandle ProgressBarTimerHandle;
	
	UAnimInstance* AnimInstance{};
	AJellyManager* JellyManager;
	
	AItemBox* ItemBox;
	AItemBox* prevItemBox;
	AFuseBox* FuseBox;
	
	ABomb* m_Bomb;

};
