// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SendLoginPacket();
	UFUNCTION(BlueprintCallable)
	bool GetLoginResult();
	UFUNCTION(BlueprintCallable)
	void DisplayErrorLog();
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* id_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* pwd_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* errorMessage;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* errorMessage2;
};
