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

	gameinstance = Cast<UMyGameInstance>(GetGameInstance());
	if (gameinstance && previewCharacterActor) {
		FString role = gameinstance->GetRoleF();
		PressSelectButton(1);
		if (role == "Chaser") {
			Button->SetVisibility(ESlateVisibility::Visible);
			Button_1->SetVisibility(ESlateVisibility::Visible);
			Button_2->SetVisibility(ESlateVisibility::Hidden);
			Button_3->SetVisibility(ESlateVisibility::Hidden);
			Button_4->SetVisibility(ESlateVisibility::Hidden);
			FButtonStyle ButtonStyle = Button->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[5]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[5]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[5]);
			Button->SetStyle(ButtonStyle);
			
			ButtonStyle = Button_1->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[6]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[6]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[6]);
			Button_1->SetStyle(ButtonStyle);
			previewCharacterActor->ApplyChaserCharacterSkeletonMesh(6);
		}
		else if(previewCharacterActor) {
			Button->SetVisibility(ESlateVisibility::Visible);
			Button_1->SetVisibility(ESlateVisibility::Visible);
			Button_2->SetVisibility(ESlateVisibility::Visible);
			Button_3->SetVisibility(ESlateVisibility::Visible);
			Button_4->SetVisibility(ESlateVisibility::Visible);
			
			
			
			FButtonStyle ButtonStyle = Button->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[0]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[0]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[0]);
			Button->SetStyle(ButtonStyle);

			ButtonStyle = Button_1->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[1]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[1]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[1]);
			Button_1->SetStyle(ButtonStyle);

			ButtonStyle = Button_2->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[2]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[2]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[2]);
			Button_2->SetStyle(ButtonStyle);

			ButtonStyle = Button_3->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[3]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[3]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[3]);
			Button_3->SetStyle(ButtonStyle);

			ButtonStyle = Button_4->WidgetStyle;
			ButtonStyle.SetNormal(FSlateBrush());
			ButtonStyle.Normal.SetResourceObject(ImageArray[4]);
			ButtonStyle.Hovered.SetResourceObject(ImageArray[4]);
			ButtonStyle.Pressed.SetResourceObject(ImageArray[4]);
			Button_4->SetStyle(ButtonStyle);

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
		gameinstance->SendRolePacket();
	}
}
