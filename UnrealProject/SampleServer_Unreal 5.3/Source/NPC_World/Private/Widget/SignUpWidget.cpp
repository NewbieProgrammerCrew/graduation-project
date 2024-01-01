
#include "../../Public/Widget/SignUpWidget.h"
#include "../../Public/Manager/MyGameInstance.h"

void USignUpWidget::DisplayError(int ErrorCode)
{
	if (ErrorCode == 100) {
		errorMessage->SetText(FText::FromString(TEXT("이미 존재하는 아이디 입니다.")));
		errorMessage->SetVisibility(ESlateVisibility::Visible);
	}
	else if (ErrorCode == 101) {
		errorMessage->SetText(FText::FromString(TEXT("이미 존재하는 닉네임 입니다.")));
		errorMessage->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		errorMessage->SetVisibility(ESlateVisibility::Hidden);
	}

}
void USignUpWidget::CloseWidget()
{
	id_box->SetText(FText::FromString(""));
	pwd_box->SetText(FText::FromString(""));
	confirm_pwd_box->SetText(FText::FromString(""));
	nickname_box->SetText(FText::FromString(""));
	RemoveFromParent();
}
bool USignUpWidget::SendSignUpPacket()
{
	errorMessage->SetVisibility(ESlateVisibility::Hidden);
	FText id = id_box->GetText();
	FText pwd = pwd_box->GetText();
	FText confirm_pwd = confirm_pwd_box->GetText();
	FText name = nickname_box->GetText();

	if (id.IsEmpty() || pwd.IsEmpty() || confirm_pwd.IsEmpty() || name.IsEmpty()) {
		return false;
	}
	else if (pwd.ToString() != confirm_pwd.ToString()) {
		errorMessage->SetText(FText::FromString(TEXT("비밀번호가 일치하지 않습니다.")));
		errorMessage->SetVisibility(ESlateVisibility::Visible);
		return false;
	}
	else  if (pwd.ToString() == confirm_pwd.ToString()) {
		UGameInstance* gameInstance = GetWorld()->GetGameInstance();
		if (gameInstance) {
			UMyGameInstance* myGameInstance = Cast<UMyGameInstance>(gameInstance);
			if (myGameInstance) {
				myGameInstance->SendSignUpPacket(id.ToString(), pwd.ToString(), name.ToString());
				return true;
			}
		}
	}
	return false;
}