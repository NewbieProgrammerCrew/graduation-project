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
	
	//getter
	UFUNCTION(BlueprintCallable, Category = "Status")
	FString GetRole();
	UFUNCTION(BlueprintCallable, Category = "Status")
	int GetFuseCount();
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
	int GetWhichFuseBoxOpen();
	bool IsCharacterFalling();
private:
	void BindWidget();
private:
	ACharacter* MyCharacter;
	APlayerController* OwnerController;
	UCharacterMovementComponent* MovementComp;
	FString m_role;
	float m_CurrSpeed;
	float m_FullHP;
	float m_CurrHP;
	int m_FuseCount;
	int m_fuseId;
	bool m_Jump;
	bool m_OpenFuseBox;

};
