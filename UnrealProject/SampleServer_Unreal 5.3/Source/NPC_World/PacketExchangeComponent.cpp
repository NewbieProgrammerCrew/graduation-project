// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketExchangeComponent.h"
#include "GameFramework/Character.h"
#include "DataUpdater.h"
#include "NetworkingThread.h"
#include "MyPlayerController.h"

// Sets default values for this component's properties
UPacketExchangeComponent::UPacketExchangeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UPacketExchangeComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UPacketExchangeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    if (_Controller == nullptr) {
        AActor* OwnerActor = GetOwner();
        if (ACharacter* MyCharacter = Cast<ACharacter>(OwnerActor)) {
            _Controller = Cast<AMyPlayerController>(MyCharacter->Controller);
        }
        if (_Controller == nullptr) return;
    }
     if (Network == nullptr && _Controller) {
          Network = reinterpret_cast<FSocketThread*>(_Controller->Network);
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPacketExchangeComponent::SendHittedPacket()  // 피격정보 전달
{
    AActor* OwnerActor = GetOwner();
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(OwnerActor->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater && Network) {
        float hp = DataUpdater->GetCurrentHP();

        CS_HITTED_PACKET packet;
        packet.size = sizeof(CS_HITTED_PACKET);
        packet.type = CS_HITTED;
        packet.hp = hp;

        WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
        if (!wsa_over_ex) {
            return;
        }
        
        if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            delete wsa_over_ex;
        }
    }
}

