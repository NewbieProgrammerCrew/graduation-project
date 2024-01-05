// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseGun.h"
#include "ExplosiveGun.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API AExplosiveGun : public ABaseGun
{
	GENERATED_BODY()
public:

	AExplosiveGun() :ABaseGun() {}
	AExplosiveGun(int count) :ABaseGun(count) {};
	virtual void Fire() override;
private:
	int damage;
	 
};
