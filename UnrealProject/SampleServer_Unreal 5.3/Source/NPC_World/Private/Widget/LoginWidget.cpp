// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Widget/LoginWidget.h"
#include "../../Public/Manager/MyGameInstance.h"

void ULoginWidget::SendLoginPacket()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (myGameInstance) {
			FText id = id_box->GetText();
			FText pwd = pwd_box->GetText();
			errorMessage2->SetVisibility(ESlateVisibility::Hidden);
			errorMessage->SetVisibility(ESlateVisibility::Hidden);

			if (!id.IsEmpty() && !pwd.IsEmpty()) {
				myGameInstance->SendLogInPacket(id.ToString(), pwd.ToString());
			}
			else if(id.IsEmpty()){
				
				FString error = FString(TEXT("아이디를 입력해주세요"));
				FText errorText = FText::FromString(error);
				errorMessage->SetText(errorText);
				errorMessage->SetVisibility(ESlateVisibility::Visible);
			}
			else if (pwd.IsEmpty()) {
				FString error = FString(TEXT("비밀번호를 입력해주세요"));
				FText errorText = FText::FromString(error);
				errorMessage->SetText(errorText);
				errorMessage->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}
bool ULoginWidget::GetLoginResult()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (myGameInstance) {
			return myGameInstance->GetLoginResult();
		}
	}
	return false;
}

void ULoginWidget::InitializeTextBlock()
{
	id_box->SetText(FText::FromString(""));
	pwd_box->SetText(FText::FromString(""));
}

void ULoginWidget::DisplayErrorLog()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (myGameInstance) {
			if (myGameInstance->GetErrorLog() == 102) {
				errorMessage->SetVisibility(ESlateVisibility::Hidden);
				errorMessage2->SetText(FText::FromString(TEXT("아이디와 비밀번호를 확인해주세요.")));
				errorMessage2->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}
