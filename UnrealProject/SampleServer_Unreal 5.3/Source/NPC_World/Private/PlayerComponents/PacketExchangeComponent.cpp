// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/PlayerComponents/PacketExchangeComponent.h"
#include "GameFramework/Character.h"
#include "../../Public/PlayerComponents/DataUpdater.h"
#include "NetworkingThread.h"
#include "../../Public/PlayerController/Ch_PlayerController.h"

// Sets default values for this component's properties
UPacketExchangeComponent::UPacketExchangeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UPacketExchangeComponent::BeginPlay()
{
	Super::BeginPlay();

    DataUpdater = Cast<UDataUpdater>(GetOwner()->GetComponentByClass(UDataUpdater::StaticClass()));
}
void UPacketExchangeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    if (_Controller == nullptr) {
        AActor* OwnerActor = GetOwner();
        if (ACharacter* MyCharacter = Cast<ACharacter>(OwnerActor)) {
            _Controller = Cast<ACh_PlayerController>(MyCharacter->Controller);
        }
        if (_Controller == nullptr) return;
    }
    if (!Network && _Controller) {
        Network = reinterpret_cast<FSocketThread*>(_Controller->Network);
    }
    if (DataUpdater) {
        if (DataUpdater->IsCharacterFalling()) {
            SendMovePacket();
            didjump = true;
        }
    }
    if (didjump) {
        if (!DataUpdater->IsCharacterFalling()) {
            SendMovePacket(didjump);
            didjump = false;
        }
    }
}
void UPacketExchangeComponent::SendHittedPacket()
{
    AActor* OwnerActor = GetOwner();
   
    if (OwnerActor && Network) {
        if (DataUpdater && DataUpdater->GetRole() == "Chaser") {

            CS_HIT_PACKET packet;

            FVector pos = OwnerActor->GetActorLocation();
            FRotator CurrentRotation = OwnerActor->GetActorRotation();

            float rx = CurrentRotation.Pitch;
            float ry = CurrentRotation.Yaw;
            float rz = CurrentRotation.Roll;

            packet.size = sizeof(CS_HIT_PACKET);

            packet.x = pos.X;
            packet.y = pos.Y;
            packet.z = pos.Z;

            packet.rx = rx;
            packet.ry = ry;
            packet.rz = rz;

            packet.type = CS_HIT;

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
}
void UPacketExchangeComponent::SendAttackPacket(int id)
{
    AActor* OwnerActor = GetOwner();
    if (OwnerActor && Network) {
        if (DataUpdater->GetRole() == "Runner") {

        }
        else if (DataUpdater->GetRole() == "Chaser") {
            CS_ATTACK_PACKET packet;
            FVector pos = OwnerActor->GetActorLocation();

            FRotator CurrentRotation = OwnerActor->GetActorRotation();
            float rx = CurrentRotation.Pitch;
            float ry = CurrentRotation.Yaw;
            float rz = CurrentRotation.Roll;

            packet.size = sizeof(CS_ATTACK_PACKET);
            packet.id = id;

            packet.x = pos.X;
            packet.y = pos.Y;
            packet.z = pos.Z;

            packet.rx = rx;
            packet.ry = ry;
            packet.rz = rz;

            packet.type = CS_ATTACK;

            WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
            if (!wsa_over_ex) {
                return;
            }
            if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0,
                &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                delete wsa_over_ex;
            }
        }

    }
}
void UPacketExchangeComponent::SendInteractionPacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
        if (!lp) return;

        UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        int fusebox_id = local_Dataupdater->GetWhichFuseBoxOpen();
        //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("F key : %d"),fusebox_id));

        if (fusebox_id >= 0) {
            if (OwnerPawn && Network) {
                CS_PUT_FUSE_PACKET packet;
                packet.size = sizeof(CS_PUT_FUSE_PACKET);
                packet.type = CS_PUT_FUSE;
                packet.fuseBoxIndex = fusebox_id;
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
    }
}

void UPacketExchangeComponent::SendGetItemPacket(int item_id)
{
    if (Network) {
        CS_ITEM_PICKUP_PACKET packet;
        packet.size = sizeof(CS_ITEM_PICKUP_PACKET);
        packet.type = CS_PICKUP;
        packet.itemId = item_id;

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

void UPacketExchangeComponent::SendMovePacket(int speed, bool didYouJump)
{
    AActor* OwnerActor = GetOwner();
    if (OwnerActor && Network) {

        float rx = 0;
        float ry = 0;
        float rz = 0;

        FRotator CurrentRotation = OwnerActor->GetActorRotation();
        rx = CurrentRotation.Pitch;
        ry = CurrentRotation.Yaw;
        rz = CurrentRotation.Roll;
        FVector CurrentPos = OwnerActor->GetActorLocation();
        CS_MOVE_PACKET packet;
        packet.size = sizeof(CS_MOVE_PACKET);
        packet.x = CurrentPos.X;
        packet.y = CurrentPos.Y;
        packet.z = CurrentPos.Z;
        float m_currSpeed = 0;

        if(speed < 0)
            m_currSpeed = DataUpdater->GetCurrentSpeed();
        
        packet.rx = rx;
        packet.ry = ry;
        packet.rz = rz;
        packet.speed = m_currSpeed;
        if(!didYouJump)
            packet.jump = DataUpdater->IsCharacterFalling();
        else
            packet.jump = false;
        packet.type = CS_MOVE;

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

void UPacketExchangeComponent::CalculateMovement()
{

}

void UPacketExchangeComponent::CalculateSpeed()
{

}

void UPacketExchangeComponent::CalculateFalling()
{

}
