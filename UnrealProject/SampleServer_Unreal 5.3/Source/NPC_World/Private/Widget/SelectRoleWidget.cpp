// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Widget/SelectRoleWidget.h"
#include "Kismet/GameplayStatics.h"

void USelectRoleWidget::submitSelectedRole(int type)
{

	UGameInstance* gameInstance = GetWorld()->GetGameInstance();

	if (gameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(gameInstance);
		if (myGameInstance) {
			myGameInstance->SetRole(type);
		}
	}
	if (WidgetManager) {
		WidgetManager->CallBindSelectCharacterWidget();
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

void USelectRoleWidget::Init()
{
	UpdateNickName();

	UWorld* worldref = GetWorld();
	if (worldref == nullptr) return;
	AActor* actor = UGameplayStatics::GetActorOfClass(worldref, AWidgetManager::StaticClass());
	if (actor == nullptr) return;
	WidgetManager = Cast<AWidgetManager>(actor);
	if (WidgetManager == nullptr) return;
}
