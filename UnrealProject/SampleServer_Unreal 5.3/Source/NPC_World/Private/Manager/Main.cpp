// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/Main.h"
#include "NetworkingThread.h"
#include <future>
#include "PlayerController/Ch_PlayerController.h"
#include <Kismet/GameplayStatics.h>

AMain::AMain() 
{
  PrimaryActorTick.bCanEverTick = true;
}
void AMain::SendMapLoadedPacket()
{
	GameInstance->SendMapLoadedPacket();
}
void AMain::BeginPlay()
{
	Super::BeginPlay();
	init_finish = false;
}
void AMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!init_finish) Init();
	if (LoadedMap) return;
	if(Network && Network->_PlayerManager){
		SendMapLoadedPacket();
		LoadedMap = true;
	}
}

void AMain::ChangeCamera_EscLocCamera()
{
	localPlayerController->SetViewTargetWithBlend(cameraActor);
}

void AMain::Init()
{
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (!GameInstance) {
		UE_LOG(LogTemp, Error, TEXT("GameInstance를 캐스팅할 수 없습니다."));
		return;
	}

	if (!GameInstance->Network) {
		// 아이피 입력 위젯 추가 => 아이피가 입력 => 네트워크 설정
		GameInstance->LoadIPAddress();
	}
	if (!GameInstance->Network) {
		return;
	}
	Network = GameInstance->Network;
	if (!Network->_MainClass) {
		Network->_MainClass = this;
	}

	cameraActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (cameraActor == nullptr) {
		return;
	}

	cameraActor = UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass());
	if (!cameraActor) {
		UE_LOG(LogTemp, Error, TEXT("카메라 액터를 찾을 수 없습니다."));
		return;
	}

	playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!playerController) {
		UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러를 찾을 수 없습니다."));
		return;
	}

	localPlayerController = Cast<ACh_PlayerController>(playerController);
	if (!localPlayerController) {
		UE_LOG(LogTemp, Error, TEXT("플레이어 컨트롤러를 캐스팅할 수 없습니다."));
	}
	init_finish = true;
}
