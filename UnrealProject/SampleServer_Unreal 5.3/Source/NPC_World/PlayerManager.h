#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h" 
#include <concurrent_queue.h>
#include <array>
#include "../../../../Server/ServerTest/ServerTest/protocol.h"
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
	TMap<FString, TSubclassOf<ACharacter>> PlayerBPMap;
	UPROPERTY(EditAnywhere)
	class AMain* Main;
	class FSocketThread* Network;


	void SetPlayerQueue(SC_ADD_PLAYER_PACKET* packet);
	void Set_Player_Move_Queue(SC_MOVE_PLAYER_PACKET* MovePacket);
	void Set_Player_Attack_Queue(SC_ATTACK_PLAYER_PACKET* AttackPacket);
	void Set_Player_Hitted_Queue(SC_HITTED_PACKET* HittedPacket);
	void Set_Player_Dead_Queue(SC_DEAD_PACKET* DEADPacket);
	void Set_Player_Item_Pickup_Queue(SC_PICKUP_PACKET* PickupPacket);
	void Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* RemovePacket);

	void Spawn_Player(SC_ADD_PLAYER_PACKET packet);
	void Play_Attack_Animation(SC_ATTACK_PLAYER_PACKET packet);
	void Player_Hitted(SC_HITTED_PACKET hitted_player);
	void Player_Dead(SC_DEAD_PACKET dead_player);
	void Player_Item_Pickup(SC_PICKUP_PACKET item_pickup_player);

	void Set_Player_Location(int citizen_id, FVector Packet_Location, FRotator Rotate);
	void Remove_Player(int _id);

private:
	concurrency::concurrent_queue <SC_ADD_PLAYER_PACKET> PlayerQueue;
	concurrency::concurrent_queue <SC_MOVE_PLAYER_PACKET> Player_Move_Queue;
	concurrency::concurrent_queue<SC_ATTACK_PLAYER_PACKET> Player_Attack_Queue;
	concurrency::concurrent_queue<SC_HITTED_PACKET> Player_Hitted_Queue;
	concurrency::concurrent_queue <SC_DEAD_PACKET> Player_Dead_Queue;
	concurrency::concurrent_queue <SC_PICKUP_PACKET> Player_Item_Pickup_Queue;
	concurrency::concurrent_queue <SC_REMOVE_PLAYER_PACKET> Player_Remove_Queue;

	float cur_speed = 0;
	bool cur_jump = false;
	float InterpolationFactor = 0;
	float m_InterpolationFactor = 0;
	std::array<AActor*, 1000> Player;
};