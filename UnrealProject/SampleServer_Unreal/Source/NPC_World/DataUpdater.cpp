#include "DataUpdater.h"

// Sets default values for this component's properties
UDataUpdater::UDataUpdater()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDataUpdater::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpeed = 0.0f;
}

void UDataUpdater::UpdateSpeedData(float Speed)
{
	CurrentSpeed = Speed;
}


// Called every frame
void UDataUpdater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

