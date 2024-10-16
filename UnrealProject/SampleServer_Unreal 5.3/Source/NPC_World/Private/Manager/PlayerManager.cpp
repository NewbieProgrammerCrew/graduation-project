// Fill out your copyright notice in the Description page of Project Settings.


#include "../../Public/Manager/PlayerManager.h"
#include "../../Public/Actors/BaseRunner.h"
#include "../../Public/Actors/BaseChaser.h"


#include <string>
#include <sstream>
#include <thread>
#include "Kismet/GameplayStatics.h"

#include "../../Public/PlayerController/Ch_PlayerController.h"
#include "../../Public/PlayerComponents/DataUpdater.h"
#include "../../Public/Manager/Main.h"
#include "Engine/LevelScriptActor.h"
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
    TArray<FVector> Group1;
    Group1.Add(FVector(10903.0f, 2857.0f, 3125.0f));
    Group1.Add(FVector(1908.0f, 10545.0f, 1143.0f));
    Group1.Add(FVector(16953.0f, 10545.0f, 1143.0f));
    Group1.Add(FVector(8302.0f, 17215.0f, 1143.0f));
    Group1.Add(FVector(5180.0f, 11717.0f, 1143.0f));

    // 그룹 2 데이터
    TArray<FVector> Group2;
    Group2.Add(FVector(6608.0f, 14923.0f, 770.0f));
    Group2.Add(FVector(17550.0f, 23118.0f, 708.0f));
    Group2.Add(FVector(20173.0f, 10456.0f, 708.0f));
    Group2.Add(FVector(6433.0f, 6775.0f, 777.0f));
    Group2.Add(FVector(6433.0f, 25816.0f, 800.0f));

    // 그룹 3 데이터
    TArray<FVector> Group3;
    Group3.Add(FVector(2955.0f, 2183.0f, 511.0f));
    Group3.Add(FVector(2356.0f, 12491.0f, 727.0f));
    Group3.Add(FVector(12870.0f, 12491.0f, 727.0f));
    Group3.Add(FVector(12870.0f, 6673.0f, 727.0f));
    Group3.Add(FVector(20971.0f, 6673.0f, 727.0f));

    // 그룹 4 데이터
    TArray<FVector> Group4;
    Group4.Add(FVector(7846.0f, 11619.0f, 752.0f));
    Group4.Add(FVector(14465.0f, 17410.0f, 752.0f));
    Group4.Add(FVector(14465.0f, 12900.0f, 752.0f));
    Group4.Add(FVector(12750.0f, 6652.0f, 752.0f));
    Group4.Add(FVector(16445.0f, 8883.0f, 752.0f));

    // 그룹을 데이터 배열에 추가
    PositionArray.Add(Group1);
    PositionArray.Add(Group2);
    PositionArray.Add(Group3);
    PositionArray.Add(Group4);
    
    actor->GetWorld();
	Main = Cast<AMain>(actor);
	Network = nullptr;





}

void APlayerManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Network == nullptr && Main->init_finish) {
        Network = reinterpret_cast<FSocketThread*>(Main->Network);
        if (!Network) return;
        Network->_PlayerManager = this;
        lobby_id = Main->GameInstance->GetMyLobbyID();
        game_id = Main->GameInstance->GetMyGameID();
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
         /*   rx = CurrentRotation.Roll;
            ry = CurrentRotation.Pitch;
            rz = CurrentRotation.Yaw;*/

            FRotator Rotation = FRotator(move_player.ry, move_player.rz, move_player.rx);
            FVector location = FVector(move_player.x, move_player.y, move_player.z);
            double pitch = move_player.pitch;
            cur_speed = move_player.speed;
            cur_jump = move_player.jump;
            Set_Player_Location(move_player.id, location, Rotation, pitch);
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
    SC_CANNON_FIRE_PACKET FireCannonPlayer;
    while (!Player_Fire_Cannon_Queue.empty()) {
        if (Player_Fire_Cannon_Queue.try_pop(FireCannonPlayer)) {
            Player_Fire_Cannon(FireCannonPlayer);
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
    SC_ESCAPE_PACKET escape_player;
    while (!Player_Escape_Queue.empty()) {
        if (Player_Escape_Queue.try_pop(escape_player)) {
            Player_Escape(escape_player);
        }
    }
    SC_CHASER_WIN_PACKET chaserWin;
    while (!Chaser_Win_Queue.empty()) {
        if (Chaser_Win_Queue.try_pop(chaserWin)) {
            Chaser_Win(chaserWin);
        }
    }
    SC_SKILL_CHOOSED_PACKET choosed_student_player;
    while (!Student_Choosed_Skill_Queue.empty()) {
        if (Student_Choosed_Skill_Queue.try_pop(choosed_student_player)) {
            Choosed_Skill_Student_Player(choosed_student_player);
        }
    }
   /* SC_RESET_FUSE_BOX_PACKET Reset_FuseBox_player;
    while (!Player_Reset_FuseBox_Queue.empty()) {
        if (Player_Reset_FuseBox_Queue.try_pop(Reset_FuseBox_player)) {
            Player_Reset_FuseBox(Reset_FuseBox_player);
        }
    }*/
    SC_PICKUP_BOMB_PACKET Bomb_Pickup_player;
    while (!Player_Bomb_Pickup_Queue.empty()) {
        if (Player_Bomb_Pickup_Queue.try_pop(Bomb_Pickup_player)) {
            Player_Bomb_Pickup(Bomb_Pickup_player);
        }
    }


    SC_PICK_UP_EXPLOSION_PACKET Explosion_Pickup_player;
    while (!Player_Explosion_Pickup_Queue.empty()) {
        if (Player_Explosion_Pickup_Queue.try_pop(Explosion_Pickup_player)) {
            Player_Explosion_Pickup(Explosion_Pickup_player);
        }
    }
    SC_PICK_UP_STUN_PACKET Stun_Pickup_player;
    while (!Player_Stun_Pickup_Queue.empty()) {
        if (Player_Stun_Pickup_Queue.try_pop(Stun_Pickup_player)) {
            Player_Stun_Pickup(Stun_Pickup_player);
        }
    }
    SC_PICK_UP_INK_PACKET Ink_Pickup_player;
    while (!Player_Ink_Pickup_Queue.empty()) {
        if (Player_Ink_Pickup_Queue.try_pop(Ink_Pickup_player)) {
            Player_Ink_Pickup(Ink_Pickup_player);
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
    SC_USE_SKILL_PACKET skill_player;
    while (!Player_Use_Skill_Queue.empty()) {
        if (Player_Use_Skill_Queue.try_pop(skill_player)) {
            Player_Use_Skill(skill_player);
        }
    }
    SC_REMOVE_PLAYER_PACKET remove_player;
    while (!Player_Remove_Queue.empty()) {
        if (Player_Remove_Queue.try_pop(remove_player)) {
            Remove_Player(remove_player.id);
        }
    }
}

void APlayerManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    Network = nullptr;
}

void APlayerManager::Choosed_Skill_Student_Player(SC_SKILL_CHOOSED_PACKET packet)
{  
    ABaseRunner* runner = Cast<ABaseRunner>(Player[game_id]);
    if (runner) {
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(runner->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->SetSkillType(packet.skill_type);
        }
        runner->CallSelectedSkillEvent();
    }
}

void APlayerManager::Spawn_Player(SC_ADD_PLAYER_PACKET AddPlayer) {
    if (!(Main && Main->GameInstance))return;
    int mapid = Main->GameInstance->GetMapId() - 1;
    UWorld* uworld = GetWorld();
    if (!uworld) return;

    if (!(std::string(AddPlayer.role).size() && AddPlayer.id >= 0)) return;

    ACharacter* SpawnedCharacter = nullptr;
    int characterTypeNumer = AddPlayer.charactorNum;
   
  
    if (Player[AddPlayer.id] != nullptr) {
        UpdateCharacterPosition(AddPlayer.id, PositionArray[mapid][AddPlayer.id % 5]);
    }
    else {
        SpawnNewCharacter(uworld, AddPlayer, characterTypeNumer, PositionArray[mapid][AddPlayer.id %5]);
    }
    AsyncTask(ENamedThreads::GameThread, [uworld]() {
        ALevelScriptActor* LevelScriptActor = uworld->GetLevelScriptActor();
        if (LevelScriptActor) {
            UFunction* Function = LevelScriptActor->FindFunction(FName("DestroyLoadWidget"));
            if (Function) {
                LevelScriptActor->ProcessEvent(Function, nullptr);
            }
        }
        });
}

void APlayerManager::SpawnNewCharacter(UWorld* uworld, SC_ADD_PLAYER_PACKET& AddPlayer, int characterTypeNumer, FVector pos) 
{
    if (!PlayerBPMap.Contains(characterTypeNumer)) return;

    FVector SpawnLocation(pos);
    ACharacter* SpawnedCharacter = uworld->SpawnActor<ACharacter>(PlayerBPMap[characterTypeNumer], SpawnLocation, FRotator::ZeroRotator);
    if (!SpawnedCharacter) return; // 스폰 실패 시 종료

    Player[AddPlayer.id] = Cast<AActor>(SpawnedCharacter);
    if (Player[AddPlayer.id]) {
        if (!PossessCharacter(AddPlayer.id)) {
            Main->SendMapLoadedPacket();
        }
        else UpdateCharacterData(AddPlayer, characterTypeNumer);
    }
}

bool APlayerManager::PossessCharacter(int playerId)
{
    if (Network && playerId == Network->my_game_id) {
        APlayerController* RawController = UGameplayStatics::GetPlayerController(this, 0);
        ACh_PlayerController* MyController = Cast<ACh_PlayerController>(RawController);
        if (MyController) {
            MyController->Possess(Cast<APawn>(Player[playerId]));
            return true;
        }
    }
    return false;
}

void APlayerManager::UpdateCharacterData(SC_ADD_PLAYER_PACKET& AddPlayer, int characterTypeNumer) 
{
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[AddPlayer.id]->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetRole(FString(AddPlayer.role));
        DataUpdater->SetHPData(AddPlayer._hp);
        DataUpdater->SetCharacterType(characterTypeNumer);
    }
}

void APlayerManager::UpdateCharacterPosition(int playerId, FVector position)
{
    Player[playerId]->SetActorHiddenInGame(false);
    Player[playerId]->SetActorLocation(position);
}


void APlayerManager::Set_Player_Location(int _id, FVector Packet_Location, FRotator Rotate, double pitch)
{
    if (_id >= 0 && Player[_id] != nullptr) {
        if (Player[_id]->GetWorld() && Player[_id]->IsValidLowLevel()) {

            UWorld* world = Player[_id]->GetWorld();
            float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(world);
            UDataUpdater* DataUpdater = Cast<UDataUpdater>(Player[_id]->GetComponentByClass(UDataUpdater::StaticClass()));
            if (DataUpdater) {
                DataUpdater->SetCameraPitchValue(pitch);
            }
            if (_id != Network->my_game_id) {

                FVector currentLocation = Player[_id]->GetActorLocation();
                FVector targetLocation = Packet_Location;

                // 보간 계수 업데이트
                InterpolationFactor += 0.5f * DeltaTime;
                InterpolationFactor = FMath::Clamp(InterpolationFactor, 0.f, 1.f);

                FVector interpolatedLocation = FMath::Lerp(currentLocation, targetLocation, InterpolationFactor);
                Player[_id]->SetActorLocation(interpolatedLocation, false, nullptr, ETeleportType::TeleportPhysics);


                // 데이터 업데이터 컴포넌트 업데이트
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

void APlayerManager::Player_Escape(SC_ESCAPE_PACKET packet)
{
    int id = packet.id;
    if (id < 0 || id > Player.size() - 1) return;

    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (playerInstance) {

        ABaseChaser* chaser = Cast<ABaseChaser>(playerInstance);
        ABaseRunner* runner = Cast<ABaseRunner>(playerInstance);
        if (chaser) {
            if (Network->my_game_id == packet.id) {
                chaser->SetGameResult(false);
            }
        }
        else if (runner) {
            if (Network->my_game_id == packet.id) {
                runner->SetGameResult(packet.runner_win);
            }
        }
        AsyncTask(ENamedThreads::GameThread, [playerInstance]()
            {
                if (playerInstance && playerInstance->IsValidLowLevel()) {
                    UFunction* AddWidgetEvent = playerInstance->FindFunction(FName("GameResultWidget"));
                    if (AddWidgetEvent) {
                        playerInstance->ProcessEvent(AddWidgetEvent, nullptr);
                    }
                }
            });
    }
   
    Main->ChangeCamera_EscLocCamera();
}
void APlayerManager::Chaser_Win(SC_CHASER_WIN_PACKET packet)
{
    int id = game_id;
    if (id < 0 || id > Player.size() - 1) return;
    
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (playerInstance) {

        ABaseChaser* chaser = Cast<ABaseChaser>(playerInstance);
        ABaseRunner* runner = Cast<ABaseRunner>(playerInstance);
        if (chaser) {
                chaser->SetGameResult(true);
           
        }
        else if (runner) {
            runner->SetGameResult(false);

        }
        AsyncTask(ENamedThreads::GameThread, [playerInstance]()
            {
                if (playerInstance && playerInstance->IsValidLowLevel()) {
                    UFunction* AddWidgetEvent = playerInstance->FindFunction(FName("GameResultWidget"));
                    if (AddWidgetEvent) {
                        playerInstance->ProcessEvent(AddWidgetEvent, nullptr);
                    }
                }
            });
    }

    Main->ChangeCamera_EscLocCamera();
}



void APlayerManager::Play_Attack_Animation(SC_ATTACK_PLAYER_PACKET packet)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[packet.id]);
    if (playerInstance) {
        ABaseChaser* chaser = Cast<ABaseChaser>(playerInstance);
        if (chaser) {
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
            UFunction* ApplyStunEvent = nullptr;
            UFunction* ApplyBlindEvent = nullptr;

            if (hitted_player.bombType == BombType::Stun) {
                ApplyStunEvent = playerInstance->FindFunction(FName("StunDamage"));
                ApplyBlindEvent = nullptr;
            }
            if (hitted_player.bombType == BombType::Blind) {
                ApplyBlindEvent = playerInstance->FindFunction(FName("BlindDamage"));
                ApplyStunEvent = nullptr;
            }
            if (hitted_player.bombType == BombType::Explosion) {
                ApplyBlindEvent = nullptr;
                ApplyStunEvent = nullptr;
            }
            if (ApplyDamageEvent) {
                playerInstance->ProcessEvent(ApplyDamageEvent, nullptr);
            }
            if (ApplyStunEvent) {
                playerInstance->ProcessEvent(ApplyStunEvent, nullptr);
            }  
            if (ApplyBlindEvent) {
                playerInstance->ProcessEvent(ApplyBlindEvent, nullptr);
            }
        }
    }
}

void APlayerManager::Player_FUSE_Pickup(SC_PICKUP_FUSE_PACKET item_pickup_player)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[item_pickup_player.id]);
    if (!playerInstance) return;

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
        ACharacter* playerInstance = Cast<ACharacter>(Player[Network->my_game_id]);
        if (!playerInstance) return;
        UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
        if (DataUpdater) {
            DataUpdater->UpdatePortalStatus(ratio);
        }
    }
}

void APlayerManager::Player_Bomb_Pickup(SC_PICKUP_BOMB_PACKET item_pickup_player)
{
    int id = item_pickup_player.id;
    if (id < 0) return;
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (!playerInstance) return;

    UFunction* BombUpdateWidgetEvent = playerInstance->FindFunction(FName("BombCountEvent"));
    if (BombUpdateWidgetEvent) {
        playerInstance->ProcessEvent(BombUpdateWidgetEvent, nullptr);
    }
    ABaseRunner* runnerInstance = Cast<ABaseRunner>(playerInstance);
    if (runnerInstance) {
        runnerInstance->PlayEarnBomb();
        runnerInstance->EquipBomb(BombType(item_pickup_player.bombType));
    }
    
}

void APlayerManager::Player_Explosion_Pickup(SC_PICK_UP_EXPLOSION_PACKET packet)
{
    int id = packet.c_id;
    if (id < 0) return;
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (!playerInstance) return;
  
    ABaseRunner* runnerInstance = Cast<ABaseRunner>(playerInstance);
    if (runnerInstance) {
        runnerInstance->PlayEarnBomb();
        runnerInstance->EquipBomb(BombType::Explosion);
    }
}

void APlayerManager::Player_Stun_Pickup(SC_PICK_UP_STUN_PACKET packet)
{
    int id = packet.c_id;
    if (id < 0) return;
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (!playerInstance) return;

    ABaseRunner* runnerInstance = Cast<ABaseRunner>(playerInstance);
    if (runnerInstance) {
        runnerInstance->PlayEarnBomb();
        runnerInstance->EquipBomb(BombType::Stun);
    }
}

void APlayerManager::Player_Ink_Pickup(SC_PICK_UP_INK_PACKET packet)
{
    int id = packet.c_id;
    if (id < 0) return;
    ACharacter* playerInstance = Cast<ACharacter>(Player[id]);
    if (!playerInstance) return;

    ABaseRunner* runnerInstance = Cast<ABaseRunner>(playerInstance);
    if (runnerInstance) {
        runnerInstance->PlayEarnBomb();
        runnerInstance->EquipBomb(BombType::Blind);
    }
}

void APlayerManager::Player_Fire_Cannon(SC_CANNON_FIRE_PACKET fireCannonPlayer)
{
    int _id = fireCannonPlayer.id;
    int bombType = fireCannonPlayer.bomb_type;
    FVector pos = { fireCannonPlayer.x, fireCannonPlayer.y, fireCannonPlayer.z };
    float rx = fireCannonPlayer.rx;
    float ry = fireCannonPlayer.ry;
    float rz = fireCannonPlayer.rz;
    
    if (_id >= 0 && Player[_id] != nullptr) {
        ABaseRunner* RunnerInstance = Cast<ABaseRunner>(Player[_id]);
        if (RunnerInstance) {
            RunnerInstance->ShootCannon(pos, {rx,ry,rz});
            RunnerInstance->DecreaseBomb();
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
            RunnerInstance->PlayAimAnim();
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
            RunnerInstance->StopAimEvent();
        }
    }
}

void APlayerManager::Player_Use_Skill(SC_USE_SKILL_PACKET skill_player)
{
    int _id = skill_player.id;
    if (_id >= 0 && Player[_id] != nullptr) {
        
        ABaseChaser* chaserInstance = Cast<ABaseChaser>(Player[_id]);
        ABaseRunner* runnerInstance = Cast<ABaseRunner>(Player[_id]);
        
        if (chaserInstance) {
            UDataUpdater* DataUpdater = Cast<UDataUpdater>(chaserInstance->GetComponentByClass(UDataUpdater::StaticClass()));
            if (DataUpdater) {
                DataUpdater->SetSkillType(skill_player.skill_type);
            }
            chaserInstance->ActivateSkill();
        }
        if (runnerInstance) {
            UDataUpdater* DataUpdater = Cast<UDataUpdater>(runnerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
            if (DataUpdater) {
                DataUpdater->SetSkillType(skill_player.skill_type);
            }
            runnerInstance->ActivateSkill();
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
//void APlayerManager::Player_Reset_FuseBox(SC_RESET_FUSE_BOX_PACKET packet)
//{
//    int id = packet.chaserId;
//    if (id >= 0 && Player[id] != nullptr) {
//        ABaseChaser* ChaserInstance = Cast<ABaseChaser>(Player[id]);
//        if (ChaserInstance) {
//            if (id == Network->my_id)
//                ChaserInstance->PlayResetFirstPersonAnimation();
//            else
//                ChaserInstance->PlayResetThirdPersonAnimation();
//        }
//    }
//}

void APlayerManager::Player_Dead(SC_DEAD_PACKET dead_player)
{
    ACharacter* playerInstance = Cast<ACharacter>(Player[dead_player.id]);
    if (!playerInstance) return;
    UDataUpdater* DataUpdater = Cast<UDataUpdater>(playerInstance->GetComponentByClass(UDataUpdater::StaticClass()));
    if (DataUpdater) {
        DataUpdater->SetCurrentHP(dead_player._hp);
    }
    ACh_PlayerController* MyController = Cast<ACh_PlayerController>(playerInstance->Controller);
    if (MyController) MyController->isAlive = false;
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
    ACh_PlayerController* MyController = Cast<ACh_PlayerController>(BaseChaserInstance->Controller);
    if (MyController) MyController->isAlive = true;
    
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
void APlayerManager::Set_Player_Escape_Queue(SC_ESCAPE_PACKET* packet)
{
    Player_Escape_Queue.push(*packet);
}
void APlayerManager::Set_Chaser_Win_Queue(SC_CHASER_WIN_PACKET* packet)
{
    Chaser_Win_Queue.push(*packet);
}
void APlayerManager::Set_Player_Resurrect_Queue(SC_CHASER_RESURRECTION_PACKET* packet)
{
    Player_Resurrection_Queue.push(*packet);
}
void APlayerManager::Set_Player_Fuse_Pickup_Queue(SC_PICKUP_FUSE_PACKET* packet)
{
    Player_Fuse_Pickup_Queue.push(*packet);
}
void APlayerManager::Set_Player_Bomb_Pickup_Queue(SC_PICKUP_BOMB_PACKET* packet)
{
    Player_Bomb_Pickup_Queue.push(*packet);
}

//debug
void APlayerManager::Set_Player_Explosion_Pickup_Queue(SC_PICK_UP_EXPLOSION_PACKET* packet)
{
    Player_Explosion_Pickup_Queue.push(*packet);
}

void APlayerManager::Set_Player_Ink_Pickup_Queue(SC_PICK_UP_INK_PACKET* packet)
{
    Player_Ink_Pickup_Queue.push(*packet);
}

void APlayerManager::Set_Player_Stun_Pickup_Queue(SC_PICK_UP_STUN_PACKET* packet)
{
    Player_Stun_Pickup_Queue.push(*packet);
}


void APlayerManager::Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* packet)
{
	Player_Remove_Queue.push(*packet);
}
void APlayerManager::Set_Player_Aiming_Queue(SC_AIM_STATE_PACKET* packet)
{
    Player_Aim_Queue.push(*packet);
}

void APlayerManager::Set_Player_FireCannon_Queue(SC_CANNON_FIRE_PACKET* packet)
{
    Player_Fire_Cannon_Queue.push(*packet);
}

void APlayerManager::Set_Player_Idle_Queue(SC_IDLE_STATE_PACKET* packet)
{
    Player_Idle_Queue.push(*packet);
}
void APlayerManager::Set_Player_Use_Skill_Queue(SC_USE_SKILL_PACKET* packet)
{
    Player_Use_Skill_Queue.push(*packet);
}

void APlayerManager::Set_Student_Player_Choosed_Skill_Queue(SC_SKILL_CHOOSED_PACKET* packet)
{
    Student_Choosed_Skill_Queue.push(*packet);
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
//void APlayerManager::Set_Player_Reset_FuseBox_Queue(SC_RESET_FUSE_BOX_PACKET* packet)
//{
//    Player_Reset_FuseBox_Queue.push(*packet);
//}
//void APlayerManager::Set_Player_Use_Bomb_Queue(SC_USE_Bomb_PACKET* packet)
//{
//    Player_Use_Bomb_Queue.push(*packet);
//}


//////////////////

