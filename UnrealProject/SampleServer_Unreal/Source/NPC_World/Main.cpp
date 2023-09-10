// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "NetworkingThread.h"
#include <future>
#include <mutex>
#include <Kismet/GameplayStatics.h>


AMain::AMain()
{
	PrimaryActorTick.bCanEverTick = true;

}
FRunnableThread* NetworkThread;
void AMain::BeginPlay()
{
	Super::BeginPlay();
	Network = new FSocketThread();

	const TCHAR* TCHARString = L"127.0.0.1";
	int32 TCHARLength = FCString::Strlen(TCHARString);
	int32 BufferSize = WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, nullptr, 0, nullptr, nullptr);
	ZeroMemory(&Network->IPAddress, 20);
	WideCharToMultiByte(CP_UTF8, 0, TCHARString, TCHARLength, Network->IPAddress, BufferSize, nullptr, nullptr);
	Network->_MainClass = this;
	NetworkThread = FRunnableThread::Create(Network, TEXT("MyThread"), 0, TPri_BelowNormal);
}
void AMain::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (Network) Network->Stop();
}
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

