// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManager.h"
#include <string>
#include <sstream>
#include <thread>
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Main.h"
#include "NetworkingThread.h"
#include "DataUpdater.h"

APlayerManager::APlayerManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlayerManager::BeginPlay()
{
	Super::BeginPlay();
    
    memset(Player.data(), 0, sizeof(Player));

	UWorld* world = GetWorld();
	if (!world) {
		return;
	}
	AActor* actor = UGameplayStatics::GetActorOfClass(world, AMain::StaticClass());
	if (actor == nullptr) {
		return;
	}
	actor->GetWorld();
	Main = Cast<AMain>(actor);
	Network = nullptr;
}

void APlayerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Network == nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("NetworkSet")));

        Network = reinterpret_cast<FSocketThread*>(Main->Network);
        Network->_PlayerManager = this;
        UE_LOG(LogTemp, Log, TEXT("Manager connect"));
        Main->SendMapLoadedPacket();
    }
    SC_ADD_PLAYER_PACKET AddPlayer;
    while (!PlayerQueue.empty()) {
        if (PlayerQueue.try_pop(AddPlayer)) {
            Spawn_Player(AddPlayer);
        }
    }
    SC_MOVE_PLAYER_PACKET move_player;
    while (!Player_Move_Queue.empty()) {
        if (Player_Move_Queue.try_pop(move_player)) {
            FRotator Rotation = FRotator(move_player.rx, move_player.ry, move_player.rz);
            FVector location = FVector(move_player.x, move_player.y, move_player.z);
            cur_speed = move_player.speed;
            Set_Player_Location(move_player.id, location, Rotation);
        }
    }

    SC_ATTACK_PLAYER_PACKET attack_player;
    while (!Player_Attack_Queue.empty()) {
        if (Player_Attack_Queue.try_pop(attack_player)) {
            Play_Attack_Animation(attack_player);
        }
    }
    SC_HITTED_PACKET hitted_player;
    while (!Player_Hitted_Queue.empty()) {
        if (Player_Hitted_Queue.try_pop(hitted_player)) {
            Player_Hitted(hitted_player);
        }
    }
    SC_DEAD_PACKET dead_player;
    while (!Player_Dead_Queue.empty()) {
        if (Player_Dead_Queue.try_pop(dead_player)) {
            Player_Dead(dead_player);
        }
    }
    SC_PICKUP_PACKET item_Pickup_player;
    while (!Player_Item_Pickup_Queue.empty()) {
        if (Player_Item_Pickup_Queue.try_pop(item_Pickup_player)) {
            Player_Item_Pickup(item_Pickup_player);
        }
    }

    SC_REMOVE_PLAYER_PACKET remove_player;
    while (!Player_Remove_Queue.empty()) {
        if (Player_Remove_Queue.try_pop(remove_player)) {
            Remove_Player(remove_player.id);
        }
    }
}

void APlayerManager::Spawn_Player(SC_ADD_PLAYER_PACKET AddPlayer) {
    
        UWorld* uworld = nullptr;
        while (!uworld) {
                uworld = GetWorld();
        }
        ACharacter* SpawnedCharacter = nullptr;
        if (std::string(AddPlayer.role).size() && AddPlayer.id >= 0 && Player[AddPlayer.id] == nullptr && PlayerBPMap.Contains(AddPlayer.role)) {
            SpawnedCharacter = uworld->SpawnActor<ACharacter>(PlayerBPMap[AddPlayer.role],FVector(0, 0, 100), FRotator(0.0f, 0.0f, 0.0f));
            if (SpawnedCharacter) {
                    Player[AddPlayer.id] = Cast<AActor>(SpawnedCharacter);
            }
            if (Network && AddPlayer.id == Network->my_id) {
                APlayerController* RawController = UGameplayStatics::GetPlayerController(this, 0);
                AMyPlayerController* MyController = Cast<AMyPlayerController>(RawController);
                if (MyController) {
                    MyController->Possess(Cast<APawn>(SpawnedCharacter));
                }
            }
            if (Player[AddPlayer.id]) {
                UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[AddPlayer.id]->GetComponentByClass(
                        UDataUpdater::StaticClass()));
                if (DataUpdater) {
                    DataUpdater->SetRole(FString(AddPlayer.role));
                    DataUpdater->SetHPData(AddPlayer._hp);
                }
            }
        } else if (std::string(AddPlayer.role).size() && AddPlayer.id >= 0 && Player[AddPlayer.id]) {
            Player[AddPlayer.id]->SetActorHiddenInGame(false);
            Player[AddPlayer.id]->SetActorLocation(FVector(0, 0, 300));
        }
}

void APlayerManager::Play_Attack_Animation(SC_ATTACK_PLAYER_PACKET packet) 
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[packet.id]);
    if (playerInstance) {
        UFunction* AtkCustomEvent = playerInstance->FindFunction(FName("AtkAnimEvent"));
        if (AtkCustomEvent) {
            playerInstance->ProcessEvent(AtkCustomEvent, nullptr);
        }
    }
}

void APlayerManager::Player_Hitted(SC_HITTED_PACKET hitted_player)
{
    int _id = hitted_player.id;
    if (_id >= 0 && Player[_id] != nullptr) {
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->SetCurrentHP(hitted_player._hp);
        }

        ACharacter* playerInstance = Cast<ACharacter>(Player[_id]);
        if (playerInstance) {
            UFunction* ApplyDamageEvent = playerInstance->FindFunction(FName("ApplyDamage"));
            if (ApplyDamageEvent) {
                playerInstance->ProcessEvent(ApplyDamageEvent, nullptr);
            }
        }
    }
}

void APlayerManager::Player_Dead(SC_DEAD_PACKET dead_player)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[dead_player.id]);
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetCurrentHP(dead_player._hp);
    }
    UFunction* DeadCustomEvent = playerInstance->FindFunction(FName("DeadEvent"));
    if (DeadCustomEvent) {
        playerInstance->ProcessEvent(DeadCustomEvent, nullptr);
    }
}

void APlayerManager::Player_Item_Pickup(SC_PICKUP_PACKET item_pickup_player)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[item_pickup_player.id]);
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetCurrentFuseCount();
    }
    UFunction* FuseCountEvent = playerInstance->FindFunction(FName("FuseCountEvent"));
    if (FuseCountEvent) {
        playerInstance->ProcessEvent(FuseCountEvent, nullptr);
    }
}


void APlayerManager::Set_Player_Location(int _id, FVector Packet_Location, FRotator Rotate)
{
    if (_id >= 0 && Player[_id] != nullptr) {
        if (Player[_id]->GetWorld() && Player[_id]->IsValidLowLevel()) {

            UWorld* world = Player[_id]->GetWorld();
            float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(world);
            
            
            if (_id != Network->my_id) {
                UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
                if (DataUpdater) {
                    DataUpdater->SetCurrentSpeed(cur_speed);
                }
                InterpolationFactor += 0.5f * DeltaTime;
                InterpolationFactor = FMath::Clamp(InterpolationFactor, 0.f, 1.f);
                if (InterpolationFactor >= 0.99f) {
                    Player[_id]->SetActorLocation(Packet_Location);
                   
                }
                else {
                    // 위치 보간
                    FVector InterpolatedLocation = FMath::CubicInterp(Player[_id]->GetActorLocation(),
                        FVector::ZeroVector, Packet_Location, FVector::ZeroVector, InterpolationFactor);
                    Player[_id]->SetActorLocation(InterpolatedLocation);
                }
                FQuat CurrentQuat = Player[_id]->GetActorQuat();
                FQuat TargetQuat = FQuat(Rotate);
                FQuat InterpolatedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, InterpolationFactor);

                Player[_id]->SetActorRotation(InterpolatedQuat.Rotator());
            
            } else {
                ACharacter* CharacterInstance = Cast<ACharacter>(Player[_id]);
                if (CharacterInstance) {
                    CharacterInstance->bUseControllerRotationYaw = true;
                    if (CharacterInstance->GetController()) {
                        FQuat CurrentQuat = CharacterInstance->GetController()->GetControlRotation().Quaternion();
                        FQuat TargetQuat = FQuat(Rotate);
                        FQuat InterpolatedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, 0.4);
                        CharacterInstance->GetController()->SetControlRotation(InterpolatedQuat.Rotator());

                    }
                }
            }
        }
    }
}

void APlayerManager::Remove_Player(int _id)
{
	if (Player[_id] != nullptr) {
		Player[_id]->Destroy();
		Player[_id] = nullptr;
	}
}

void APlayerManager::SetPlayerQueue(SC_ADD_PLAYER_PACKET* packet)
{
	PlayerQueue.push(*packet);
}
void APlayerManager::Set_Player_Move_Queue(SC_MOVE_PLAYER_PACKET* packet)
{
	Player_Move_Queue.push(*packet);
}
void APlayerManager::Set_Player_Attack_Queue(SC_ATTACK_PLAYER_PACKET* packet) 
{
    Player_Attack_Queue.push(*packet);
}
void APlayerManager::Set_Player_Hitted_Queue(SC_HITTED_PACKET* packet)
{
    Player_Hitted_Queue.push(*packet);
}
void APlayerManager::Set_Player_Dead_Queue(SC_DEAD_PACKET* packet)
{
    Player_Dead_Queue.push(*packet);
}
void APlayerManager::Set_Player_Item_Pickup_Queue(SC_PICKUP_PACKET* packet)
{
    Player_Item_Pickup_Queue.push(*packet);
}
void APlayerManager::Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* packet)
{
	Player_Remove_Queue.push(*packet);
}