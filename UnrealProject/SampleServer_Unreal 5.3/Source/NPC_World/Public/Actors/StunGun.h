// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseGun.h"
#include "StunGun.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API AStunGun : public ABaseGun
{
	GENERATED_BODY()
public:
	AStunGun():ABaseGun(){
	
	}
	AStunGun(int count) :ABaseGun(count) {}
	
	virtual void Fire() override;
private:
	int damage;
};
