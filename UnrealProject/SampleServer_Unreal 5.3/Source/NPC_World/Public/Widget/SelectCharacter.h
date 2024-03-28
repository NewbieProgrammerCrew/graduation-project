// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "../Manager/MyGameInstance.h"
#include "../Actors/PreviewCharacter.h"
#include "SelectCharacter.generated.h"

/**
 * 
 */
UCLASS()
class NPC_WORLD_API USelectCharacter : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	UFUNCTION(BlueprintCallable)
	void PressSelectButton(int ChType);
	UFUNCTION(BlueprintCallable)
	void PressOkButton();
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_3;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_4;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* RunnerBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* ChaserBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_5;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_6;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* WaitingPhrase;

private:
	UMyGameInstance* gameinstance;
	APreviewCharacter* previewCharacterActor;
};
