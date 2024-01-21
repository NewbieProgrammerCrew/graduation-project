#include "../../Public/PlayerComponents/DataUpdater.h"


// Sets default values for this component's properties
UDataUpdater::UDataUpdater()
{
	PrimaryComponentTick.bCanEverTick = true;
	m_FullHP = 0;
	m_CurrHP = 0;
	m_FuseCount = 0;
	m_Jump = true;
	m_Jump = false;
	m_fuseId = -1;
}


void UDataUpdater::BeginPlay()
{
	Super::BeginPlay();
	m_CurrSpeed = 0.0f;
	MyCharacter = Cast<ACharacter>(GetOwner());
	MovementComp = MyCharacter->GetCharacterMovement();
}

void UDataUpdater::SetRole(FString role)
{
	m_role = role;
}

void UDataUpdater::SetCurrentSpeed(float Speed)
{
	float InterpolationSpeed = 0.4f;
	if (Speed > 0.00001f) {
		m_CurrSpeed = FMath::Lerp(m_CurrSpeed, Speed, InterpolationSpeed);
	} else {
		m_CurrSpeed = 0;
	}
}
void UDataUpdater::SetHPData(float hp) 
{
	m_FullHP = hp;
    m_CurrHP = m_FullHP;
	BindWidget();
}

void UDataUpdater::SetCurrentHP(float hp) 
{
	m_CurrHP = hp;
}

void UDataUpdater::SetIncreaseFuseCount()
{
	if (m_FuseCount == 0)
		++m_FuseCount;
}

void UDataUpdater::SetIncreasePistolCount()
{
	++m_PistolCount;
}

void UDataUpdater::SetDecreasePistolCount()
{
	if (m_PistolCount > 0)
		--m_PistolCount;
}

void UDataUpdater::SetDecreaseFuseCount()
{
	if (m_FuseCount > 0)
		--m_FuseCount;
}

void UDataUpdater::SetOnJumpStatus(bool result)
{
	m_Jump = result;
}

void UDataUpdater::SetFuseBoxOpenability(bool result)
{
	m_OpenFuseBox = result;
}

void UDataUpdater::SetFuseBoxOpenAndInstall(int fuse_id)
{
	m_fuseId = fuse_id;
}

void UDataUpdater::ResetItemBoxOpeningProgress()
{
	m_OpeningItemBoxRatio = 0;
	ACharacter* Own = Cast<ACharacter>(GetOwner());
	if (Own)
		OwnerController = Cast<APlayerController>(Own->GetController());
	if (OwnerController) {
		UFunction* ResetOpeningItemBoxStatusWidget = GetOwner()->FindFunction(FName("ResetOpeningItemBoxStatusWidget"));
		if (ResetOpeningItemBoxStatusWidget) {
			GetOwner()->ProcessEvent(ResetOpeningItemBoxStatusWidget, nullptr);
		}
	}
}

void UDataUpdater::SetItemBoxOpeningProgress(float progress)
{
	m_OpeningItemBoxRatio = progress;
	ACharacter* Own = Cast<ACharacter>(GetOwner());
	if (Own)
		OwnerController = Cast<APlayerController>(Own->GetController());
	if (OwnerController) {
		UFunction* updateOpeningItemBoxStatusWidget = GetOwner()->FindFunction(FName("UpdateOpeningItemBoxStatusWidget"));
		if (updateOpeningItemBoxStatusWidget) {
			GetOwner()->ProcessEvent(updateOpeningItemBoxStatusWidget, nullptr);
		}
	}
}

void UDataUpdater::SetAimStatus()
{
	m_aim = true;
}

void UDataUpdater::SetNaviStatus()
{
	m_aim = false;
}

FString UDataUpdater::GetRole()
{
	return m_role;
}

int UDataUpdater::GetFuseCount()
{
	return m_FuseCount;
}

int UDataUpdater::GetPistolCount()
{
	return 0;
}

float UDataUpdater::GetCurrentSpeed()
{
	return m_CurrSpeed;
}

float UDataUpdater::GetCurrentHP() 
{ 
	return m_CurrHP; 
}

float UDataUpdater::GetFullHP()
{
	return m_FullHP;
}

void UDataUpdater::GetJumpStatus(bool& result)
{
	result = m_Jump;
}

bool UDataUpdater::CheckFuseBoxOpenability()
{
	return m_OpenFuseBox;
}

bool UDataUpdater::GetAimStatus()
{
	return m_aim;
}

int UDataUpdater::GetWhichFuseBoxOpen()
{
	return m_fuseId;
}

// Called every frame
void UDataUpdater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) {
		return;
	}
	APlayerController* Owner = Cast<APlayerController>(OwnerPawn->GetController());
	if (!Owner) {
		return; // 컨트롤러가 PlayerController가 아니면 함수를 종료
	}
	if (!MyCharacter) {
		MyCharacter = Cast<ACharacter>(OwnerPawn);
	}
	if (!MovementComp && MyCharacter) {
		MovementComp = MyCharacter->GetCharacterMovement();
	}

	if (MovementComp) {
		FVector SpeedV = FVector(MovementComp->Velocity.X, MovementComp->Velocity.Y, 0);
		SetCurrentSpeed(SpeedV.Size());
		m_Jump = IsCharacterFalling();
	}

}

bool UDataUpdater::IsCharacterFalling()
{
	if (!MovementComp && MyCharacter) {
		MovementComp = MyCharacter->GetCharacterMovement();
	}
	if (MovementComp) {
		FVector Velocity = MovementComp->Velocity;
		bool IsMovingUpwards = Velocity.Z > 0.00001f;
		bool IsMovingDownwards = Velocity.Z < -0.000001f;
	
		if ((IsMovingUpwards || IsMovingDownwards)) {
			return true;
		}
	}
	return false;
}

void UDataUpdater::BindWidget()
{
	ACharacter* Own = Cast<ACharacter>(GetOwner());
	if(Own)
		OwnerController = Cast<APlayerController>(Own->GetController());
	if (OwnerController) {
		if (GetRole() == "Runner") {
			UFunction* AddWidgetEvent = GetOwner()->FindFunction(FName("AddWidgetEvent"));
			if (AddWidgetEvent) {
				GetOwner()->ProcessEvent(AddWidgetEvent, nullptr);
			}
		}
	}
}

void UDataUpdater::UpdateFuseStatusWidget()
{
	ACharacter* Own = Cast<ACharacter>(GetOwner());
	if (Own)
		OwnerController = Cast<APlayerController>(Own->GetController());
	if (OwnerController) {
		UFunction* updateFuseStatusWidget = GetOwner()->FindFunction(FName("UpdateFuseStatusWidget"));
		if (updateFuseStatusWidget) {
			GetOwner()->ProcessEvent(updateFuseStatusWidget, nullptr);
		}
	}
}

void UDataUpdater::UpdatePortalStatus(float ratio)
{
	m_PortalRatio = ratio;
	ACharacter* Own = Cast<ACharacter>(GetOwner());
	if (Own)
		OwnerController = Cast<APlayerController>(Own->GetController());
	if (OwnerController) {
		UFunction* updateFuseStatusWidget = GetOwner()->FindFunction(FName("UpdatePortalStatusWidget"));
		if (updateFuseStatusWidget) {
			GetOwner()->ProcessEvent(updateFuseStatusWidget, nullptr);
		}
	}
}

float UDataUpdater::GetPortalStatus()
{
	return m_PortalRatio;
}

float UDataUpdater::GetItemBoxOpeningProgress()
{
	return m_OpeningItemBoxRatio;
}

void UDataUpdater::SetCurrentOpeningItem(int itemtype)
{
	m_CurrentItemOpening = itemtype;
}

void UDataUpdater::SetCurrentOpeningItemIndex(int itemIdx)
{
	m_CurrentItemOpeningIndex = itemIdx;
}

int UDataUpdater::GetCurrentOpeningItem()
{
	return m_CurrentItemOpening;
}

int UDataUpdater::GetCurrentOpeningItemIndex()
{
	return m_CurrentItemOpeningIndex;
}

void UDataUpdater::SetGunAvailability(bool b)
{
	hasGunAvailable = b;
}
bool UDataUpdater::GetGunAvailability()
{
	return hasGunAvailable;
}

void UDataUpdater::SetTempGunType(int GunType)
{
	m_tguntype = GunType;
}

int UDataUpdater::GetTempGunType()
{
	return m_tguntype;
}

