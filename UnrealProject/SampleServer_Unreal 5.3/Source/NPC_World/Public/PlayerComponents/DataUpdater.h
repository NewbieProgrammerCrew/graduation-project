#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameframework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataUpdater.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NPC_WORLD_API UDataUpdater : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UDataUpdater();

public:
	//setter
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetCurrentSpeed(float Speed);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetRole(FString Role);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetHPData(float hp);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetCurrentHP(float hp);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetIncreaseFuseCount();
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetDecreaseFuseCount();
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetOnJumpStatus(bool result);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetFuseBoxOpenability(bool result);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetFuseBoxOpenAndInstall(int fuse_id);
	UFUNCTION(BlueprintCallable, Category = "Status")
	void ResetItemBoxOpeningProgress();
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetItemBoxOpeningProgress(float progress);

	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetAimStatus();
	UFUNCTION(BlueprintCallable, Category = "Status")
	void SetNaviStatus();
	void SetCameraPitchValue(double pitch);
	void SetBombEquipState(bool equip);
	
	//getter
	UFUNCTION(BlueprintCallable, Category = "Status")
	FString GetRole();
	UFUNCTION(BlueprintCallable, Category = "Status")
	int GetFuseCount();	
	UFUNCTION(BlueprintCallable, Category = "Status")
	int GetBombCount();
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetCurrentSpeed();
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetCurrentHP();
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetFullHP();
	UFUNCTION(BlueprintCallable, Category = "Status")
	void GetJumpStatus(bool& curr_jump);
	UFUNCTION(BlueprintCallable, Category = "Status")
	bool CheckFuseBoxOpenability();
	UFUNCTION(BlueprintCallable, Category = "Status")
	bool GetAimStatus();
	UFUNCTION(BlueprintCallable, Category = "Status")
	bool hasBomb();
	int GetWhichFuseBoxOpen();
	bool IsCharacterFalling();

	void UpdateFuseStatusWidget();
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void UpdatePortalStatus(float ratio);
	UFUNCTION(BlueprintCallable, Category = "Portal")
	float GetPortalStatus();
	UFUNCTION(BlueprintCallable, Category = "Status")
	float GetItemBoxOpeningProgress();

	UFUNCTION(BlueprintCallable, Category = "ItemOpen")
	void SetCurrentOpeningItem(int itemtype);
	UFUNCTION(BlueprintCallable, Category = "ItemOpen")
	void SetCurrentOpeningItemIndex(int itemIdx);
	void ClearOpeningBoxData();

	int GetCurrentOpeningItem();
	int GetCurrentOpeningItemIndex();

	UFUNCTION(BlueprintCallable, Category = "hasBomb")
	void SetBombAvailability(bool b);
	UFUNCTION(BlueprintCallable, Category = "hasBomb")
	bool GetBombAvailability();
	UFUNCTION(BlueprintCallable, Category = "hasBomb")
	void SetTempBombType(int BombType);	
	UFUNCTION(BlueprintCallable, Category = "hasBomb")
	void SetTempItemBoxIndex(int index);
	UFUNCTION(BlueprintCallable, Category = "hasBomb")
	int GetTempBombType();
	int GetTempItemBoxIndex();

	UFUNCTION(BlueprintCallable, Category = "CameraPitch")
	double GetCameraPitch();
	void BindWidget();
	UFUNCTION(BlueprintCallable)
	int GetCharacterType() {
		return chType;
	};
	void SetCharacterType(int t) {
		chType = t;
	};

private:
	ACharacter* MyCharacter{};
	APlayerController* OwnerController{};
	UCharacterMovementComponent* MovementComp{};
	FString m_role{};
	
	float m_CurrSpeed{};
	float m_FullHP{};
	float m_CurrHP{};
	float m_PortalRatio{};
	float m_OpeningItemBoxRatio{};

	int chType{};
	int m_CurrentItemOpening{};
	int m_CurrentItemOpeningIndex{};
	int m_FuseCount{};
	int m_BombCount{};
	int m_fuseId{};
	int m_tBombtype{};
	int m_tBombIndex{};
	bool m_Jump{};
	bool m_OpenFuseBox{};
	bool m_aim{};
	float m_pitch{};
	bool hasBombAvailable{};
	bool m_bombState{};
};
