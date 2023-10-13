// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "MyGameInstance.h"
#include "SelectRoleWidget.generated.h"

UCLASS()
class NPC_WORLD_API USelectRoleWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void submitSelectedRole(FString role);
	UFUNCTION(BlueprintCallable)
	void UpdateNickName();

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* NickName;
};
