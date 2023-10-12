// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "SignUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API USignUpWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* id_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* pwd_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* confirm_pwd_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UEditableTextBox* nickname_box;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* errorMessage;
	

	UFUNCTION(BlueprintCallable)
	bool SendSignUpPacket();
	UFUNCTION(BlueprintCallable)
	void DisplayError(int ErrorCode);
};
