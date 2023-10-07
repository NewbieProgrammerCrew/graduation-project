#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <concurrent_queue.h>
#include "../../../../Server/ServerTest/ServerTest/protocol.h"
#include "PlayerManager.generated.h"

UCLASS()
class NPC_WORLD_API APlayerManager : public AActor
{
	GENERATED_BODY()

public:
	APlayerManager();

protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<APawn> PlayerBP;

	UPROPERTY(EditAnywhere)
	class AMain* Main;
	AActor* Player[1000];
	float cur_speed = 0;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	class FSocketThread* Network;

	void SetPlayerQueue(SC_ADD_PLAYER_PACKET* packet);
	void Set_Player_Move_Queue(SC_MOVE_PLAYER_PACKET* MovePacket);
	void Set_Player_Remove_Queue(SC_REMOVE_PLAYER_PACKET* RemovePacket);
	void Spawn_Player(SC_ADD_PLAYER_PACKET packet);
	void Set_Player_Location(int citizen_id, FVector Packet_Location, FRotator Rotate);
	void Remove_Player(int _id);
	concurrency::concurrent_queue <SC_ADD_PLAYER_PACKET> PlayerQueue;
	concurrency::concurrent_queue <SC_MOVE_PLAYER_PACKET> Player_Move_Queue;
	concurrency::concurrent_queue <SC_REMOVE_PLAYER_PACKET> Player_Remove_Queue;
};