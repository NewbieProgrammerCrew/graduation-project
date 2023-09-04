// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapFileParser.generated.h"


UCLASS()
class MYPROJECT_API UMapFileParser : public UObject
{
	GENERATED_BODY()
public:
	UMapFileParser();
	UFUNCTION(BlueprintCallable)
	void ParseMapFile(const FString& FilePath);
	UFUNCTION(BlueprintCallable)
	void SpawnObjectsInWorld(UWorld* World);
};
