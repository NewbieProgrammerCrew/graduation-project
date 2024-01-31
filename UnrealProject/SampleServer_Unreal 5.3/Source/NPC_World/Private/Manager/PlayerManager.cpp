// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/PlayerManager.h"
#include "../../Public/Actors/BaseGun.h"
#include "../../Public/Actors/StunGun.h"
#include "../../Public/Actors/BaseRunner.h"
#include "../../Public/Actors/BaseChaser.h"

#include <string>
#include <sstream>
#include <thread>
#include "Kismet/GameplayStatics.h"

#include "../../Public/PlayerController/Ch_PlayerController.h"
#include "../../Public/PlayerComponents/DataUpdater.h"
#include "../../Public/Manager/Main.h"
#include "NetworkingThread.h"


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
            cur_jump = move_player.jump;
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
    SC_CHASER_RESURRECTION_PACKET resurrection_chaser;
    while (!Player_Resurrection_Queue.empty()) {
        if (Player_Resurrection_Queue.try_pop(resurrection_chaser)) {
            Player_Resurrect(resurrection_chaser);
        }
    }
    SC_OPENING_ITEM_BOX_PACKET item_Box_OpeningPlayer;
    while (!Player_Opening_ItemBox_Queue.empty()) {
        if (Player_Opening_ItemBox_Queue.try_pop(item_Box_OpeningPlayer)) {
            Player_Opening_ItemBox(item_Box_OpeningPlayer);
        }
    }
    SC_OPENING_FUSE_BOX_PACKET fuse_Box_OpeningPlayer;
    while (!Player_Opening_FuseBox_Queue.empty()) {
        if (Player_Opening_FuseBox_Queue.try_pop(fuse_Box_OpeningPlayer)) {
            Player_Opening_FuseBox(fuse_Box_OpeningPlayer);
        }
    } 
    SC_USE_GUN_PACKET useGunPlayer;
    while (!Player_Use_Gun_Queue.empty()) {
        if (Player_Use_Gun_Queue.try_pop(useGunPlayer)) {
            Player_Use_Gun(useGunPlayer);
        }
    }
    SC_STOP_OPENING_PACKET stop_OpeningPlayer;
    while (!Player_Stop_Opening_Queue.empty()) {
        if (Player_Stop_Opening_Queue.try_pop(stop_OpeningPlayer)) {
            Player_Stop_Opening_Box(stop_OpeningPlayer);
        }
    }
    SC_PICKUP_FUSE_PACKET Fuse_Pickup_player;
    while (!Player_Fuse_Pickup_Queue.empty()) {
        if (Player_Fuse_Pickup_Queue.try_pop(Fuse_Pickup_player)) {
            Player_FUSE_Pickup(Fuse_Pickup_player);
        }
    } 
    SC_PICKUP_GUN_PACKET Gun_Pickup_player;
    while (!Player_Gun_Pickup_Queue.empty()) {
        if (Player_Gun_Pickup_Queue.try_pop(Gun_Pickup_player)) {
            Player_GUN_Pickup(Gun_Pickup_player);
        }
    }
    SC_AIM_STATE_PACKET aim_player;
    while (!Player_Aim_Queue.empty()) {
        if (Player_Aim_Queue.try_pop(aim_player)) {
            Play_Aim_Animation(aim_player);
        }
    }
    SC_IDLE_STATE_PACKET idle_player;
    while (!Player_Idle_Queue.empty()) {
        if (Player_Idle_Queue.try_pop(idle_player)) {
            Play_Idle_Animation(idle_player);
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
    int characterN = AddPlayer.charactorNum;
    int filter = 5;
    
    if (std::string(AddPlayer.role).size() && AddPlayer.id >= 0 && Player[AddPlayer.id] == nullptr){
        if (std::string(AddPlayer.role) == "Chaser") {
            characterN += filter;
        }

        if (PlayerBPMap.Contains(characterN)) {
            SpawnedCharacter = uworld->SpawnActor<ACharacter>(PlayerBPMap[characterN], FVector(0, 0, 100), FRotator(0.0f, 0.0f, 0.0f));

            if (SpawnedCharacter) {
                Player[AddPlayer.id] = Cast<AActor>(SpawnedCharacter);
            }
            if (Network && AddPlayer.id == Network->my_id) {
                APlayerController* RawController = UGameplayStatics::GetPlayerController(this, 0);
                ACh_PlayerController* MyController = Cast<ACh_PlayerController>(RawController);
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
        }

    }
    else if (std::string(AddPlayer.role).size() && AddPlayer.id >= 0 && Player[AddPlayer.id]) {
        Player[AddPlayer.id]->SetActorHiddenInGame(false);
        Player[AddPlayer.id]->SetActorLocation(FVector(0, 0, 300));
    }
}

void APlayerManager::Set_Player_Location(int _id, FVector Packet_Location, FRotator Rotate)
{
    if (_id >= 0 && Player[_id] != nullptr) {
        if (Player[_id]->GetWorld() && Player[_id]->IsValidLowLevel()) {

            UWorld* world = Player[_id]->GetWorld();
            float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(world);


            if (_id != Network->my_id) {

                FVector currentLocation = Player[_id]->GetActorLocation();
                FVector targetLocation = Packet_Location;

                // 보간 계수 업데이트
                InterpolationFactor += 0.5f * DeltaTime;
                InterpolationFactor = FMath::Clamp(InterpolationFactor, 0.f, 1.f);

                FVector interpolatedLocation = FMath::Lerp(currentLocation, targetLocation, InterpolationFactor);
                Player[_id]->SetActorLocation(interpolatedLocation, false, nullptr, ETeleportType::TeleportPhysics);


                // 데이터 업데이터 컴포넌트 업데이트
                UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
                if (DataUpdater) {
                    DataUpdater->SetCurrentSpeed(cur_speed);
                    DataUpdater->SetOnJumpStatus(cur_jump);
                }
                FQuat CurrentQuat = Player[_id]->GetActorQuat();
                FQuat TargetQuat = FQuat(Rotate);
                FQuat InterpolatedQuat = FQuat::Slerp(CurrentQuat, TargetQuat, InterpolationFactor);
                Player[_id]->SetActorRotation(InterpolatedQuat.Rotator());

            }
        }
    }
}

void APlayerManager::Play_Attack_Animation(SC_ATTACK_PLAYER_PACKET packet) 
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[packet.id]);
    if (playerInstance) {
        ABaseRunner* runner = Cast<ABaseRunner>(playerInstance);
        ABaseChaser* chaser = Cast<ABaseChaser>(playerInstance);
        if (runner) {
            runner->Attack();
        }
        else if(chaser) {
            chaser->Attack();
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


void APlayerManager::Player_FUSE_Pickup(SC_PICKUP_FUSE_PACKET item_pickup_player)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[item_pickup_player.id]);
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetIncreaseFuseCount();
    }
    
    UFunction* FuseCountEvent = playerInstance->FindFunction(FName("FuseCountEvent"));
    if (FuseCountEvent) {
        playerInstance->ProcessEvent(FuseCountEvent, nullptr);
    }
}
void APlayerManager::PortalGagueUpdate(float ratio)
{
    if (Network) {
        ACharacter* playerInstance = Cast<ACharacter>(Player[Network->my_id]);
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->UpdatePortalStatus(ratio);
        }
    }
}

void APlayerManager::Player_GUN_Pickup(SC_PICKUP_GUN_PACKET item_pickup_player)
{
    int id = item_pickup_player.id;
    if (id < 0) return;
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetIncreasePistolCount();
    }
    UFunction* PistolUpdateWidgetEvent = playerInstance->FindFunction(FName("PistolCountEvent"));
    if (PistolUpdateWidgetEvent) {
        playerInstance->ProcessEvent(PistolUpdateWidgetEvent, nullptr);
    }

    ABaseRunner* runnerInstance = Cast<ABaseRunner>(playerInstance);
    if (runnerInstance)
    {
        ABaseGun* newGun = nullptr;
        UClass* BP_StunGunClass = LoadClass<ABaseGun>(nullptr, TEXT("Blueprint'/Game/Blueprints/MyActor/BP_StunGun.BP_StunGun_C'"));
        UClass* BP_ExplosiveGunClass = LoadClass<ABaseGun>(nullptr, TEXT("Blueprint'/Game/Blueprints/MyActor/BP_ExplosiveGun.BP_ExplosiveGun_C'"));
        UClass* BP_InkGunClass = LoadClass<ABaseGun>(nullptr, TEXT("Blueprint'/Game/Blueprints/MyActor/BP_InkGun.BP_InkGun_C'"));
        switch (EGunType(item_pickup_player.gun_type))
        {
        case EGunType::StunGun:
            if (BP_StunGunClass) {
                newGun = GetWorld()->SpawnActor<ABaseGun>(BP_StunGunClass);
                newGun->SetType(EGunType::StunGun);
            }
            break;
        case EGunType::ExplosiveGun:
            if (BP_ExplosiveGunClass) {
                newGun = GetWorld()->SpawnActor<ABaseGun>(BP_ExplosiveGunClass); // 폭발총 생성
                newGun->SetType(EGunType::ExplosiveGun);
            }
            break;
        case EGunType::InkGun:
            if (BP_InkGunClass) {
                newGun = GetWorld()->SpawnActor<ABaseGun>(BP_InkGunClass); // 먹물총 생성
                newGun->SetType(EGunType::InkGun);
            }
            break;
        default:
            break;
        }

        if (newGun) {
           /* ECharacterType characterType = runnerInstance->GetCharacterType();
            newGun->IncreaseBulletCountForCharacterType(characterType);*/ 
            runnerInstance->EquipGun(newGun); // 캐릭터에 권총 할당
        }
    }
    
}

void APlayerManager::Play_Aim_Animation(SC_AIM_STATE_PACKET aim_player)
{
    int _id = aim_player.id;
    if (_id >= 0 && Player[_id] != nullptr) {
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->SetAimStatus();
        }
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[_id]);
        if (RunnerInstance) {
            RunnerInstance->CallAimAnimEvent();
        }
    }
}

void APlayerManager::Play_Idle_Animation(SC_IDLE_STATE_PACKET idle_player)
{
    int _id = idle_player.id;
    if (_id >= 0 && Player[_id] != nullptr) {
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->SetNaviStatus();
        }
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[_id]);
        if (RunnerInstance) {
            RunnerInstance->CallStopAimAnimEvent();
        }
    }
}

void APlayerManager::Player_Opening_ItemBox(SC_OPENING_ITEM_BOX_PACKET packet)
{
    int id = packet.id;
    float startPoint = packet.progress;

    if (id >= 0 && Player[id] != nullptr) {
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[id]);
        if (RunnerInstance) {
            RunnerInstance->SetOpenItemBoxStartPoint(packet.progress);
            RunnerInstance->StartFillingProgressBar();
            RunnerInstance->CallBoxOpenAnimEvent();
        }
    }
}
void APlayerManager::Player_Opening_FuseBox(SC_OPENING_FUSE_BOX_PACKET packet)
{
    int id = packet.id;
    float startPoint = packet.progress;

    if (id >= 0 && Player[id] != nullptr) {
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[id]);
        if (RunnerInstance) {
            RunnerInstance->CallFuseBoxOpenAnimEvent();
        }
    }
}

void APlayerManager::Player_Stop_Opening_Box(SC_STOP_OPENING_PACKET packet)
{
    int id = packet.id;
    float startPoint = packet.progress;
    if (id >= 0 && Player[id] != nullptr) {
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[id]);
        if (RunnerInstance) {
            RunnerInstance->StopInteraction();
        }
    }
}
void APlayerManager::Player_Use_Gun(SC_USE_GUN_PACKET packet)
{
    int id = packet.id;
    if (id >= 0 && Player[id] != nullptr) {
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[id]);
        if (RunnerInstance) {
            RunnerInstance->CallDestroyGunbyTimer();
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
void APlayerManager::Player_Resurrect(SC_CHASER_RESURRECTION_PACKET player)
{
    ABaseChaser* BaseChaserInstance = Cast<ABaseChaser>(Player[player.id]);
    if (!BaseChaserInstance) return;

    UDataUpdater* DataUpdater = Cast<UDataUpdater>(BaseChaserInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    FRotator rotator{ player.rx, player.ry, player.rz };
    FVector pos{ player.x, player.y, player.z };
    if (DataUpdater) {
        DataUpdater->SetCurrentHP(player.hp);
    }
    BaseChaserInstance->UpdateTransform(rotator, pos);
    UFunction* CustomEvent = BaseChaserInstance->FindFunction(FName("ResurrentionEvent"));
    if (CustomEvent) {
        BaseChaserInstance->ProcessEvent(CustomEvent, nullptr);
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
void APlayerManager::Set_Player_Resurrect_Queue(SC_CHASER_RESURRECTION_PACKET* packet)
{
    Player_Resurrection_Queue.push(*packet);
}
void APlayerManager::Set_Player_Fuse_Pickup_Queue(SC_PICKUP_FUSE_PACKET* packet)
{
    Player_Fuse_Pickup_Queue.push(*packet);
}
void APlayerManager::Set_Player_Gun_Pickup_Queue(SC_PICKUP_GUN_PACKET* packet)
{
    Player_Gun_Pickup_Queue.push(*packet);
}
void APlayerManager::Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* packet)
{
	Player_Remove_Queue.push(*packet);
}
void APlayerManager::Set_Player_Aiming_Queue(SC_AIM_STATE_PACKET* packet)
{
    Player_Aim_Queue.push(*packet);
}

void APlayerManager::Set_Player_Idle_Queue(SC_IDLE_STATE_PACKET* packet)
{
    Player_Idle_Queue.push(*packet);
}

void APlayerManager::Set_Player_ItemBoxOpening_Queue(SC_OPENING_ITEM_BOX_PACKET* packet)
{
    Player_Opening_ItemBox_Queue.push(*packet);
}
void APlayerManager::Set_Player_FuseBoxOpening_Queue(SC_OPENING_FUSE_BOX_PACKET* packet)
{
    Player_Opening_FuseBox_Queue.push(*packet);
}
void APlayerManager::Set_Player_Stop_Opening_Queue(SC_STOP_OPENING_PACKET* packet)
{
    Player_Stop_Opening_Queue.push(*packet);
}
void APlayerManager::Set_Player_Use_Gun_Queue(SC_USE_GUN_PACKET* packet)
{
    Player_Use_Gun_Queue.push(*packet);
}


//////////////////

