
#include "../../Public/PlayerController/Ch_PlayerController.h"

#include "Kismet/GameplayStatics.h"
#include <GameFramework/CharacterMovementComponent.h>

// EnhancedInput
#include "EnhancedInput/Public/InputMappingContext.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "EnhancedInput/Public/InputActionValue.h"
//Network
#include "../../Public/Manager/Main.h"
#include "NetworkingThread.h"


void ACh_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	UWorld* worldref = GetWorld();
	if (worldref == nullptr) return;

	AActor* actor = UGameplayStatics::GetActorOfClass(worldref, AMain::StaticClass());
	if (actor == nullptr) return;

	m_Main = Cast<AMain>(actor);
	if (m_Main == nullptr) return;
	const float Interval = 1.0f / 45.0f;  // 45 FPS
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SendMovePacket, this,
				&ACh_PlayerController::SendMovePacket, Interval, true);

}

void ACh_PlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ACh_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Network == nullptr) {
		Network = reinterpret_cast<FSocketThread*>(m_Main->Network);
		Network->_MyController = this;
	}
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	if (ControlledPawn) {
		if (!ControlledPawnPacketExchange)
			ControlledPawnPacketExchange = Cast<UPacketExchangeComponent>(ControlledPawn->GetComponentByClass(UPacketExchangeComponent::StaticClass()));
	}
	
}

void ACh_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Get the local Player SubSystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	// Clear out existing mapping. add our mapping
	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(InputComponent);

	// Bind the actions

	// Completed: 눌렀다 땠을 때, Triggered 누르고 있을 때

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ACh_PlayerController::Move);
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Completed, this, &ACh_PlayerController::MoveEnd);

	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started, this, &ACh_PlayerController::Sprint);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed, this, &ACh_PlayerController::StopSprint);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &ACh_PlayerController::Jump);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Completed, this, &ACh_PlayerController::JumpEnd);
	PEI->BindAction(InputActions->InputAttack, ETriggerEvent::Started, this, &ACh_PlayerController::Attack);
	PEI->BindAction(InputActions->InputSkill, ETriggerEvent::Started, this, &ACh_PlayerController::Skill);
	PEI->BindAction(InputActions->InputInteraction, ETriggerEvent::Started, this, &ACh_PlayerController::Interaction);
	PEI->BindAction(InputActions->InputInteraction, ETriggerEvent::Completed, this, &ACh_PlayerController::InteractionEnd);

	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ACh_PlayerController::Look);

	PEI->BindAction(InputActions->InputAim, ETriggerEvent::Started, this, &ACh_PlayerController::Aiming);
	PEI->BindAction(InputActions->InputAim, ETriggerEvent::Completed, this, &ACh_PlayerController::AimEnd);
	PEI->BindAction(InputActions->InputESC, ETriggerEvent::Triggered, this, &ACh_PlayerController::EscapeGame);
}

void ACh_PlayerController::SetId(int id)
{
	m_id = id;
}

int ACh_PlayerController::GetId()
{
	return m_id;
}

void ACh_PlayerController::Move(const FInputActionValue& value)
{

	//input is a Vector 2D
	FVector2D MovementVector = value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FRotationMatrix RotationMatrix = FRotationMatrix(YawRotation);
	
	FVector ForwardDirection;
	FVector RightDirection;
	FVector UpDirection;
	RotationMatrix.GetUnitAxes(ForwardDirection,RightDirection,UpDirection); 

	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ABaseChaser* baseChaser = Cast<ABaseChaser>(ControlledPawn);
		if (baseChaser && baseChaser->bPlayResetAnim) return;
		
		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
		ABaseRunner* runner = Cast<ABaseRunner>(ControlledPawn);
	}
}
void ACh_PlayerController::MoveEnd(const FInputActionValue& value)
{
	//SendMovePacket(0);
}
void ACh_PlayerController::SendMovePacket()
{
	//패킷 전송, 현재 방향,속도, 위치 보낼 것
	//공격 패킷 전송.
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	if (ControlledPawn) {
		if(ControlledPawnPacketExchange)
			ControlledPawnPacketExchange->SendMovePacket();
	}
}

void ACh_PlayerController::Sprint(const FInputActionValue& value)
{
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
		if (MyCharacter) {
			WalkingSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed;
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;

		}
	}
}


void ACh_PlayerController::Look(const FInputActionValue& value)
{
	FVector2D LookAxisVector = value.Get<FVector2D>();
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ABaseChaser* baseChaser = Cast<ABaseChaser>(ControlledPawn);
		if (baseChaser && baseChaser->bPlayResetAnim) return;
		ControlledPawn->AddControllerYawInput(LookAxisVector.X);
		ControlledPawn->AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void ACh_PlayerController::StopSprint(const FInputActionValue& value)
{
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	else {
		ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);
		if (MyCharacter) {
			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;

		}
	}
}

void ACh_PlayerController::Jump(const FInputActionValue& value)
{
	if (!keyinput) {
		keyinput = true;
		jumpCount++;

		if (!ControlledPawn) {
			ControlledPawn = GetPawn();
		}
		ABaseChaser* baseChaser = Cast<ABaseChaser>(ControlledPawn);
		if (baseChaser && baseChaser->bPlayResetAnim) return;

		ACharacter* MyCharacter = Cast<ACharacter>(ControlledPawn);

		if (MyCharacter) {
			MyCharacter->Jump();
			SendMovePacket();
		}
		if (jumpCount == 1) {
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_JumpWindow, this, &ACh_PlayerController::ResetJumpCount, JumpKeyTimeWindow, false);
		}
		else if (jumpCount == 2) {
			if(baseChaser)
				ActivateDashSkill(baseChaser);
		
			ResetJumpCount();
		}
	}
}

void ACh_PlayerController::JumpEnd(const FInputActionValue& value)
{
	keyinput = false;
}

void ACh_PlayerController::Aiming(const FInputActionValue& value)
{
	APawn* playerInstance = GetPawn();
	UPacketExchangeComponent* PacketExchange = nullptr;
	if (playerInstance) {
		ABaseRunner* runnerInst = Cast<ABaseRunner>(playerInstance);
		if (runnerInst && runnerInst->GetBomb()) {
			PacketExchange = Cast<UPacketExchangeComponent>(runnerInst->GetComponentByClass(UPacketExchangeComponent::StaticClass()));
			if(PacketExchange)
				PacketExchange->SendAimPacket();
			UFunction* AimModeEvent = runnerInst->FindFunction(FName("SetAimMode"));
			if (AimModeEvent)
				runnerInst->ProcessEvent(AimModeEvent, nullptr);
		}
	}
}

void ACh_PlayerController::AimEnd(const FInputActionValue& value)
{
	APawn* playerInstance = GetPawn();
	UPacketExchangeComponent* PacketExchange = nullptr;
	if (playerInstance) {
		ABaseRunner* runnerInst = Cast<ABaseRunner>(playerInstance);
		if (runnerInst) {
			PacketExchange = Cast<UPacketExchangeComponent>(runnerInst->GetComponentByClass(UPacketExchangeComponent::StaticClass()));
			UFunction* StopAimCustomEvent = runnerInst->FindFunction(FName("StopAimAnimEvent"));
			PacketExchange->SendIdlePacket();
			if (StopAimCustomEvent) {
				runnerInst->ProcessEvent(StopAimCustomEvent, nullptr);
			}
		}
	}
}

void ACh_PlayerController::Skill(const FInputActionValue& value)
{
	APawn* playerInstance = GetPawn();
	if (playerInstance) {
		ABaseRunner* runnerInst = Cast<ABaseRunner>(playerInstance);
		if (runnerInst) {
			UFunction* SkillEvent = runnerInst->FindFunction(FName("SkillEvent"));
			if (SkillEvent) {
				runnerInst->ProcessEvent(SkillEvent, nullptr);
			}
		}
	}
}

void ACh_PlayerController::EscapeGame(const FInputActionValue& value)
{
	UWorld* World = GetWorld();
	if (World) {
		APlayerController* PlayerController = World->GetFirstPlayerController();
		UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
	}
}

void ACh_PlayerController::ResetFkey()
{
	F_KeyPressed = false;
}
void ACh_PlayerController::ResetJumpCount()
{
	jumpCount = 0;
}

void ACh_PlayerController::ActivateDashSkill(ABaseChaser* chaser)
{
	UFunction* DashSkillEvent = chaser->FindFunction(FName("DashSkillEvent"));
	if (DashSkillEvent) {
		chaser->ProcessEvent(DashSkillEvent, nullptr);
	}
}


void ACh_PlayerController::Attack(const FInputActionValue& value)
{
	//공격 패킷 전송.
	if (!ControlledPawn) {
		ControlledPawn = GetPawn();
	}
	if (ControlledPawn) {
		ABaseChaser* baseChaser = Cast<ABaseChaser>(ControlledPawn);
		if (baseChaser) {
			baseChaser->Attack();
			ControlledPawnPacketExchange->SendAttackPacket();
			return;
		}

		ABaseRunner* baseRunner = Cast<ABaseRunner>(ControlledPawn);
		if (baseRunner && baseRunner->GetBomb()) {
			baseRunner->Attack();
		}
	}
}

void ACh_PlayerController::Interaction(const FInputActionValue& value)
{
	F_KeyPressed = true;
	if (ControlledPawnPacketExchange) {
		ABaseChaser* baseChaser = Cast<ABaseChaser>(ControlledPawn);
		if (baseChaser && baseChaser->bPlayResetAnim) return;


		ControlledPawnPacketExchange->SendInteractionPacket();
		ControlledPawnPacketExchange->CheckEquipmentBomb();
	}
}

void ACh_PlayerController::InteractionEnd(const FInputActionValue& value)
{
	F_KeyPressed = false;
	if (ControlledPawnPacketExchange) {
		ControlledPawnPacketExchange->SendInteractionEndPacket();
	}

	APawn* playerInstance = GetPawn();
	if (playerInstance) {
		ABaseRunner* runnerInst = Cast<ABaseRunner>(playerInstance);
		if (runnerInst) {
			runnerInst->SetOpeningBox(false);
			runnerInst->SetOpeningFuseBox(false);
			runnerInst->StopInteraction();
		}
	}
}
