// Fill out your copyright notice in the Description page of Project Settings.


#include "PacketExchangeComponent.h"
#include "GameFramework/Character.h"
#include "DataUpdater.h"
#include "NetworkingThread.h"
#include "public/Ch_PlayerController.h"

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
        bool jump = DataUpdater->IsCharacterFalling();
        if(jump)
            SendMovePacket();
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

void UPacketExchangeComponent::SendMovePacket(int speed)
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
