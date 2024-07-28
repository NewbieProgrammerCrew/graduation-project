// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "PlayerInputDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class UPlayerInputDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputMove;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputSprint;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputJump;    
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputLook;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputInteraction;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputAttack;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputSkill; 
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputAim;
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    UInputAction* InputESC;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputDebug_Explosion;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputDebug_Stun;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputDebug_Ink;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* InputDebug_Portal;
};
