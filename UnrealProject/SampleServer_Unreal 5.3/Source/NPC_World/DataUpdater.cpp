#include "DataUpdater.h"

// Sets default values for this component's properties
UDataUpdater::UDataUpdater()
{
	PrimaryComponentTick.bCanEverTick = true;
	m_FullHP = 0;
    m_CurrHP = 0;
	m_FuseCount = 0;
}


void UDataUpdater::BeginPlay()
{
	Super::BeginPlay();
	CurrentSpeed = 0.0f;
}

void UDataUpdater::UpdateSpeedData(float Speed) 
{ 
	float InterpolationSpeed = 0.1f;
	CurrentSpeed = CurrentSpeed + (Speed - CurrentSpeed) * InterpolationSpeed; 

}
void UDataUpdater::UpdateRoleData(FString Role) { m_role = Role; }

void UDataUpdater::SetHPData(float hp) 
{
	m_FullHP = hp;
    m_CurrHP = m_FullHP;
}

void UDataUpdater::UpdateHPData(float hp) 
{
	m_CurrHP = hp;
}

void UDataUpdater::UpdateItemData()
{
	++m_FuseCount;
}

float UDataUpdater::GetCurrentHP() { 
	return m_CurrHP; 
}


// Called every frame
void UDataUpdater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

