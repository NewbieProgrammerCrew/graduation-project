#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataUpdater.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NPC_WORLD_API UDataUpdater : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UDataUpdater();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	//setter
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetCurrentSpeed(float Speed);
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetRole(FString Role);
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetHPData(float hp);
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetCurrentHP(float hp);
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetCurrentFuseCount();
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetOnJumpStatus(bool result);
	
	//getter
	UFUNCTION(BlueprintCallable, Category = "Data")
	FString GetRole();
	UFUNCTION(BlueprintCallable, Category = "Data")
	int GetFuseCount();
	UFUNCTION(BlueprintCallable, Category = "Data")
	float GetCurrentSpeed();
	UFUNCTION(BlueprintCallable, Category = "Data")
	float GetCurrentHP();
	UFUNCTION(BlueprintCallable, Category = "Data")
	float GetFullHP();
	UFUNCTION(BlueprintCallable, Category = "Data")
	void GetJumpStatus(bool& curr_jump);


private:
	FString m_role;
	float m_CurrSpeed;
	float m_FullHP;
	float m_CurrHP;
	int m_FuseCount;
	bool m_Jump;
};
