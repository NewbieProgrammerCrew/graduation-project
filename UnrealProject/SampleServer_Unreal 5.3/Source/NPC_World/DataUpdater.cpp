#include "DataUpdater.h"

// Sets default values for this component's properties
UDataUpdater::UDataUpdater()
{
	PrimaryComponentTick.bCanEverTick = true;
	m_FullHP = 0;
	m_CurrHP = 0;
	m_FuseCount = 0;
	m_Jump = true;
	m_Jump = false;
}


void UDataUpdater::BeginPlay()
{
	Super::BeginPlay();
	m_CurrSpeed = 0.0f;
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
}

void UDataUpdater::SetCurrentHP(float hp) 
{
	m_CurrHP = hp;
}

void UDataUpdater::SetCurrentFuseCount()
{
	++m_FuseCount;
}

void UDataUpdater::SetOnJumpStatus(bool result)
{
	m_Jump = result;
}

FString UDataUpdater::GetRole()
{
	return m_role;
}

int UDataUpdater::GetFuseCount()
{
	return m_FuseCount;
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

// Called every frame
void UDataUpdater::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

