#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataUpdater.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NPC_WORLD_API UDataUpdater : public UActorComponent
{
	GENERATED_BODY()

public:
	UDataUpdater();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, Category = "Data")
	float CurrentSpeed;

	UFUNCTION(BlueprintCallable, Category = "Data")
	void UpdateSpeedData(float Speed);
};
