// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RunnerInGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API URunnerInGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void GetAllCharactersInfo();
	void AddInGameCharacterInfo(int type);
	TArray<int> characters;
};
