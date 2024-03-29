// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SelectCharacter.h"
#include "SlateCore.h"

void USelectCharacter::NativeConstruct()
{
	UWorld* worldref = GetWorld();
	if (worldref == nullptr) return;
	AActor* actor = UGameplayStatics::GetActorOfClass(worldref, APreviewCharacter::StaticClass());
	if (actor == nullptr) return;
	previewCharacterActor = Cast<APreviewCharacter>(actor);
	if (previewCharacterActor == nullptr) return;
	WaitingPhrase->SetVisibility(ESlateVisibility::Hidden);
	gameinstance = Cast<UMyGameInstance>(GetGameInstance());
	if (gameinstance && previewCharacterActor) {
		FString role = gameinstance->GetRoleF();
		PressSelectButton(1);
		if (role == "Chaser") {
			ChaserBox->SetVisibility(ESlateVisibility::Visible);
			RunnerBox->SetVisibility(ESlateVisibility::Hidden);
			previewCharacterActor->ApplyChaserCharacterSkeletonMesh(6);
		}
		else if(previewCharacterActor) {
			RunnerBox->SetVisibility(ESlateVisibility::Visible);
			ChaserBox->SetVisibility(ESlateVisibility::Hidden);
			previewCharacterActor->ApplyRunnerCharacterSkeletonMesh(1);

		}
	}
}

void USelectCharacter::PressSelectButton(int ChType)
{
	if (gameinstance) {
		FString role = gameinstance->GetRoleF();
		if (role == "Chaser") {
			int filter = 5;
			previewCharacterActor->ApplyChaserCharacterSkeletonMesh(ChType + filter);
			gameinstance->SelectCharacter(ChType + filter);
		}
		else {
			previewCharacterActor->ApplyRunnerCharacterSkeletonMesh(ChType);
			gameinstance->SelectCharacter(ChType);
		}
	}
}

void USelectCharacter::PressOkButton()
{
	if (gameinstance) {
		WaitingPhrase->SetVisibility(ESlateVisibility::Visible);
		Button->SetIsEnabled(false);
		Button_1->SetIsEnabled(false);
		Button_2->SetIsEnabled(false);
		Button_3->SetIsEnabled(false);
		Button_4->SetIsEnabled(false);
		Button_5->SetIsEnabled(false);
		Button_6->SetIsEnabled(false);

		gameinstance->SendRolePacket();

	}
}
