// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/PlayerComponents/PacketExchangeComponent.h"
#include "GameFramework/Character.h"
#include "NetworkingThread.h"
#include "Math/UnrealMathUtility.h"
#include "../../Public/PlayerComponents/DataUpdater.h"
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
            didjump = true;
        }
    }
    if (didjump) {
        if (!DataUpdater->IsCharacterFalling()) {
            didjump = false;
        }
    }

}
void UPacketExchangeComponent::SendAttackPacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    ACh_PlayerController* lp = Cast<ACh_PlayerController>(OwnerPawn->GetController());
    if (!lp) return;
    if (OwnerPawn && Network) {

        CS_ATTACK_PACKET packet;
        FVector pos = OwnerPawn->GetActorLocation();
        FRotator CurrentRotation = OwnerPawn->GetActorRotation();

        float rx = CurrentRotation.Pitch;
        float ry = CurrentRotation.Yaw;
        float rz = CurrentRotation.Roll;

        packet.size = sizeof(CS_ATTACK_PACKET);

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

        if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            delete wsa_over_ex;
        }
    }
}

void UPacketExchangeComponent::SendCannonFirePacket(FVector cannonfrontloc, FVector dir)
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    ACh_PlayerController* lp = Cast<ACh_PlayerController>(OwnerPawn->GetController());
    if (!lp) return;
    if (OwnerPawn && Network) {
        CS_CANNON_FIRE_PACKET packet;
        packet.size = sizeof(CS_CANNON_FIRE_PACKET);
        packet.type = CS_CANNON_FIRE;
        packet.x = cannonfrontloc.X;
        packet.y = cannonfrontloc.Y;
        packet.z = cannonfrontloc.Z;
        dir.Normalize();
        packet.rx = dir.X;
        packet.ry = dir.Y;
        packet.rz = dir.Z;

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

void UPacketExchangeComponent::SendInteractionPacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        ACh_PlayerController* lp = Cast<ACh_PlayerController>(OwnerPawn->GetController());
        if (!lp) return;

        if (OwnerPawn && Network) {
            UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
            if (local_Dataupdater->GetRole() == "Runner") {
                int fusebox_id = local_Dataupdater->GetWhichFuseBoxOpen();
                int item_id = local_Dataupdater->GetCurrentOpeningItem();
                
                if (fusebox_id >= 0) {
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
                    sendPressF = true;
                    //퓨즈 감소.
                    local_Dataupdater->SetDecreaseFuseCount();
                    local_Dataupdater->UpdateFuseStatusWidget();
                }
                else if (item_id != 0) {
                    CS_PRESS_F_PACKET packet;
                    packet.size = sizeof(CS_PRESS_F_PACKET);
                    packet.type = CS_PRESS_F;
                    packet.item = item_id;
                    packet.index = local_Dataupdater->GetCurrentOpeningItemIndex();
                    local_Dataupdater->SetCurrentOpeningItem(0);
                    WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
                    if (!wsa_over_ex) {
                        return;
                    }
                    if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                        int error = WSAGetLastError();
                        delete wsa_over_ex;
                    }
                    sendPressF = true;
                }
            }
            else if (local_Dataupdater->GetRole() == "Chaser") {
                int fusebox_id = local_Dataupdater->GetWhichFuseBoxOpen();
               /* if (fusebox_id >= 0) {
                    CS_RESET_FUSE_BOX_PACKET packet;
                    packet.size = sizeof(CS_RESET_FUSE_BOX_PACKET);
                    packet.type = CS_RESET_FUSE_BOX;
                    packet.index = fusebox_id;
                    WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
                    if (!wsa_over_ex) {
                        return;
                    }

                    if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                        int error = WSAGetLastError();
                        delete wsa_over_ex;
                    }
                    sendPressF = true;
                }*/
            }
        }
    }
}

void UPacketExchangeComponent::SendEscapePacket()
{
    if (Network) {
        CS_ESCAPE_PACKET packet;
        packet.size = sizeof(CS_ESCAPE_PACKET);
        packet.type = CS_ESCAPE;
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

void UPacketExchangeComponent::SendInteractionEndPacket()
{
    if (sendPressF) {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (OwnerPawn) {
            UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
            APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
            if (!lp) return;

            if (OwnerPawn && Network) {
                if (local_Dataupdater->GetCurrentOpeningItem() == 1) {
                    local_Dataupdater->ResetItemBoxOpeningProgress();
                }
                CS_RELEASE_F_PACKET packet;
                packet.size = sizeof(CS_RELEASE_F_PACKET);
                packet.type = CS_RELEASE_F;
                packet.item = local_Dataupdater->GetCurrentOpeningItem();
                packet.index = local_Dataupdater->GetCurrentOpeningItemIndex();

                WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
                if (!wsa_over_ex) {
                    return;
                }
                if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                    int error = WSAGetLastError();
                    delete wsa_over_ex;
                }
                //초기화
                local_Dataupdater->SetFuseBoxOpenAndInstall(-1);
                local_Dataupdater->ResetItemBoxOpeningProgress();
                ACh_PlayerController* mp = Cast<ACh_PlayerController>(lp);
                if (mp)
                    mp->ResetFkey();
            }
        }
        sendPressF = false;
    }
}

void UPacketExchangeComponent::SendGetItemPacket(int item_id)
{
    if (Network) {
        CS_PICKUP_FUSE_PACKET packet;
        packet.size = sizeof(CS_PICKUP_FUSE_PACKET);
        packet.type = CS_PICKUP_FUSE;
        packet.fuseIndex = item_id;

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
void UPacketExchangeComponent::SendGetBombPacket(int bomb_type, int item_idx)
{
    if (Network) {
        CS_PICKUP_BOMB_PACKET packet;
        packet.size = sizeof(CS_PICKUP_BOMB_PACKET);
        packet.type = CS_PICKUP_BOMB;
        packet.bombType = BombType(bomb_type);
        packet.itemBoxIndex = item_idx;

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

void UPacketExchangeComponent::SendUsedBombPacket()
{
    /*AActor* OwnerActor = GetOwner();
    if (OwnerActor && Network) {
        CS_USE_Bomb_PACKET packet;
        packet.size = sizeof(CS_USE_Bomb_PACKET);
        packet.type = CS_USE_Bomb;
        WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
        if (!wsa_over_ex) {
            return;
        }

        if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
            int error = WSAGetLastError();
            delete wsa_over_ex;
        }

    }*/
}

void UPacketExchangeComponent::SendMovePacket(float PitchValue, bool didYouJump)
{
    AActor* OwnerActor = GetOwner();
    if (OwnerActor && Network) {

        float rx = 0;
        float ry = 0;
        float rz = 0;

        FRotator CurrentRotation = OwnerActor->GetActorRotation();
        rx = CurrentRotation.Roll;
        ry = CurrentRotation.Pitch;
        rz = CurrentRotation.Yaw;
        FVector CurrentPos = OwnerActor->GetActorLocation();
        CS_MOVE_PACKET packet;
        packet.size = sizeof(CS_MOVE_PACKET);
        packet.x = CurrentPos.X;
        packet.y = CurrentPos.Y;
        packet.z = CurrentPos.Z;
        packet.pitch = PitchValue;
        float m_currSpeed = m_currSpeed = DataUpdater->GetCurrentSpeed();

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

void UPacketExchangeComponent::SendAimPacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
        if (!lp) return;

        UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (Network) {
            CS_AIM_STATE_PACKET packet;
            packet.size = sizeof(CS_AIM_STATE_PACKET);
            packet.type = CS_AIM_STATE;
            WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
            if (!wsa_over_ex) {
                return;
            }

            if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                delete wsa_over_ex;
            }

        }
        local_Dataupdater->SetAimStatus();
    }
}

void UPacketExchangeComponent::SendIdlePacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
        if (!lp) return;
        UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (!local_Dataupdater) return;
        if (Network && local_Dataupdater->GetAimStatus()) {
            CS_IDLE_STATE_PACKET packet;
            packet.size = sizeof(CS_IDLE_STATE_PACKET);
            packet.type = CS_IDLE_STATE;
            WSA_OVER_EX* wsa_over_ex = new (std::nothrow) WSA_OVER_EX(OP_SEND, packet.size, &packet);
            if (!wsa_over_ex) {
                return;
            }
            if (WSASend(Network->s_socket, &wsa_over_ex->_wsabuf, 1, 0, 0, &wsa_over_ex->_wsaover, send_callback) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                delete wsa_over_ex;
            }
        }
        local_Dataupdater->SetNaviStatus();
    }
}

void UPacketExchangeComponent::SendUseSkillPacket()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
        if (!lp) return;
        if (Network) {
            CS_USE_SKILL_PACKET packet;
            packet.size = sizeof(CS_USE_SKILL_PACKET);
            packet.type = CS_USE_SKILL;
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

void UPacketExchangeComponent::CheckEquipmentBomb()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn) {
        APlayerController* lp = Cast<APlayerController>(OwnerPawn->GetController());
        if (!lp) return;
        UDataUpdater* local_Dataupdater = Cast<UDataUpdater>(OwnerPawn->GetComponentByClass(UDataUpdater::StaticClass()));
        if (!local_Dataupdater) return;
        
        if (local_Dataupdater->GetBombAvailability()) {
            int t = local_Dataupdater->GetTempBombType();
            int idx = local_Dataupdater->GetTempItemBoxIndex();
            if (t < 0 || idx < 0) return;
            SendGetBombPacket(t, idx);
            local_Dataupdater->SetTempBombType(-1);
            local_Dataupdater->SetTempItemBoxIndex(-1);
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
