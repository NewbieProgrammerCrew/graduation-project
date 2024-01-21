#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h" 
#include <concurrent_queue.h>
#include <array>
#include "../../../../../../Server/ServerTest/ServerTest/protocol.h"
#include "PlayerManager.generated.h"

UCLASS()
class NPC_WORLD_API APlayerManager : public AActor
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	APlayerManager();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int, TSubclassOf<ACharacter>> PlayerBPMap;
	UPROPERTY(EditAnywhere)
	class AMain* Main;
	class FSocketThread* Network;

	
	void SetPlayerQueue(SC_ADD_PLAYER_PACKET* packet);
	void Set_Player_Move_Queue(SC_MOVE_PLAYER_PACKET* MovePacket);
	void Set_Player_Attack_Queue(SC_ATTACK_PLAYER_PACKET* AttackPacket);
	void Set_Player_Hitted_Queue(SC_HITTED_PACKET* HittedPacket);
	void Set_Player_Dead_Queue(SC_DEAD_PACKET* DEADPacket);
	void Set_Player_Fuse_Pickup_Queue(SC_PICKUP_FUSE_PACKET * PickupPacket);
	void Set_Player_Gun_Pickup_Queue(SC_PICKUP_GUN_PACKET* PickupPacket);
	void Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* RemovePacket);
	void Set_Player_Aiming_Queue(SC_AIM_STATE_PACKET* AimPacket);
	void Set_Player_Idle_Queue(SC_IDLE_STATE_PACKET* IdlePacket);
	void Set_Player_ItemBoxOpening_Queue(SC_OPENING_ITEM_BOX_PACKET* ItemOpeningPacket);

	void Spawn_Player(SC_ADD_PLAYER_PACKET packet);
	void Set_Player_Location(int citizen_id, FVector Packet_Location, FRotator Rotate);
	
	void Play_Attack_Animation(SC_ATTACK_PLAYER_PACKET packet);
	void Player_Hitted(SC_HITTED_PACKET hitted_player);
	void Player_FUSE_Pickup(SC_PICKUP_FUSE_PACKET item_pickup_player);
	void PortalGagueUpdate(float ratio);
	void Player_GUN_Pickup(SC_PICKUP_GUN_PACKET item_pickup_player);
	void Play_Aim_Animation(SC_AIM_STATE_PACKET aim_player);
	void Play_Idle_Animation(SC_IDLE_STATE_PACKET idle_player);
	void Player_Opening_ItemBox(SC_OPENING_ITEM_BOX_PACKET packet);

	void Player_Dead(SC_DEAD_PACKET dead_player);
	void Remove_Player(int _id);


private:
	concurrency::concurrent_queue <SC_ADD_PLAYER_PACKET> PlayerQueue;
	concurrency::concurrent_queue <SC_MOVE_PLAYER_PACKET> Player_Move_Queue;
	concurrency::concurrent_queue<SC_ATTACK_PLAYER_PACKET> Player_Attack_Queue;
	concurrency::concurrent_queue<SC_HITTED_PACKET> Player_Hitted_Queue;
	concurrency::concurrent_queue <SC_DEAD_PACKET> Player_Dead_Queue;
	concurrency::concurrent_queue <SC_PICKUP_FUSE_PACKET> Player_Fuse_Pickup_Queue;
	concurrency::concurrent_queue <SC_PICKUP_GUN_PACKET> Player_Gun_Pickup_Queue;
	concurrency::concurrent_queue <SC_REMOVE_PLAYER_PACKET> Player_Remove_Queue;

	concurrency::concurrent_queue <SC_AIM_STATE_PACKET> Player_Aim_Queue;
	concurrency::concurrent_queue <SC_IDLE_STATE_PACKET> Player_Idle_Queue;
	concurrency::concurrent_queue <SC_OPENING_ITEM_BOX_PACKET> Player_Opening_ItemBox_Queue;

	float cur_speed = 0;
	bool cur_jump = false;
	float InterpolationFactor = 0;
	float m_InterpolationFactor = 0;
	std::array<AActor*, 1000> Player;
};