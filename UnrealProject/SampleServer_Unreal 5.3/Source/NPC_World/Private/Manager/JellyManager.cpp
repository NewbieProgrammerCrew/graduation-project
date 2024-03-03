// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager/JellyManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

// Sets default values
AJellyManager::AJellyManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GameInstance = nullptr;
	network = nullptr;
}

// Called when the game starts or when spawned
void AJellyManager::BeginPlay()
{
	Super::BeginPlay();
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
}
// Called every frame
void AJellyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!network && GameInstance->Network) {
		network = GameInstance->Network;
        network->_JellyManager = this;
	}
}

void AJellyManager::ExplosionParticleEvent(int idx)
{
    if(jellies[idx])
    	jellies[idx]->ExplosionEffect();
}

void AJellyManager::SendExplosionPacket(int idx)
{
    if (network) {
        CS_REMOVE_JELLY_PACKET packet;
        packet.size = sizeof(CS_REMOVE_JELLY_PACKET);
        packet.type = CS_REMOVE_JELLY;
        packet.jellyIndex = idx;
        WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
        if (!wsa_over_ex) {
            return;
        }
        if (WSASend(network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            delete wsa_over_ex;
        }

    }
}

void AJellyManager::LookAtPlayer(ACharacter* Player, int idx)
{
    if (Player) {
        FRotator PlayerRotation = Player->GetActorRotation();
        jellies[idx]->SetActorRotation(PlayerRotation);
    }
}


