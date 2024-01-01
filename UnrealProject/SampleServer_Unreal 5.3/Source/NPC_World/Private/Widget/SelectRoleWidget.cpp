// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Widget/SelectRoleWidget.h"

void USelectRoleWidget::submitSelectedRole(FString role)
{
	UGameInstance* gameInstance = GetWorld()->GetGameInstance();
	if (gameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(gameInstance);
		if (myGameInstance) {
			myGameInstance->SetRole(role);
		}
	}
}

void USelectRoleWidget::UpdateNickName()
{
	UGameInstance* gameInstance = GetWorld()->GetGameInstance();
	if (gameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(gameInstance);
		if (myGameInstance) {
			NickName->SetText(myGameInstance->GetName());
		}
	}
}
