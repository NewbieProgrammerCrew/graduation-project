// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "MyGameInstance.h"

void ULoginWidget::SendLoginPacket()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance) {
		UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (myGameInstance) {
			FText id = id_box->GetText();
			FText pwd = pwd_box->GetText();
			if (!id.IsEmpty() && !pwd.IsEmpty()) {
				errorMessage->SetVisibility(ESlateVisibility::Hidden);
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
