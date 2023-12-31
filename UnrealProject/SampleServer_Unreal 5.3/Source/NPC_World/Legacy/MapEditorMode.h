// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapEditorMode.generated.h"

/**
 * 
 */

UCLASS()
class NPC_WORLD_API AMapEditorMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetLadscapeMode();
	UFUNCTION(BlueprintCallable)
	void SetTransformMode();
	int GetCurrentMode();
private:
	bool m_LandscapeMode;
};
