// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MapFileParser.h"
#include "FilePickerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UFilePickerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString SelectFilePath;
	UPROPERTY(BlueprintReadOnly)
	UMapFileParser* FileParser;
public:
	UFUNCTION(BlueprintCallable)
	void OpenFilePicker();
};


