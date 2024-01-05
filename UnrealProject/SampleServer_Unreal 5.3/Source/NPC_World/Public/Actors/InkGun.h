// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BaseGun.h"
#include "InkGun.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API AInkGun : public ABaseGun
{
	GENERATED_BODY()
public:
	AInkGun() :ABaseGun() {}
	AInkGun(int count) :ABaseGun(count) {};
	
	virtual void Fire() override;
private:
	int damage;
	 
};
