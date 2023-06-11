// Copyright Radiant Studios

#include "RadiantPlayerController.h"

#include "Avatar.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Modes/RTGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

void ARadiantPlayerController::PlayerLoaded_Implementation()
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->PlayerLoaded();
}

void ARadiantPlayerController::S_SetPlayerStart_Implementation(ARTPlayerStart* NewPlayerStart)
{
	PlayerStart = NewPlayerStart;	
}

void ARadiantPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadiantPlayerController, PlayerStart)
}

void ARadiantPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
}

void ARadiantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnClick);
		EnhancedInputComponent->BindAction(AbilityOneAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilityOne);
		EnhancedInputComponent->BindAction(AbilityTwoAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilityTwo);
		EnhancedInputComponent->BindAction(AbilityThreeAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilityThree);
		EnhancedInputComponent->BindAction(AbilityFourAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilityFour);
		EnhancedInputComponent->BindAction(AbilityFiveAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilityFive);
		EnhancedInputComponent->BindAction(AbilitySixAction, ETriggerEvent::Started, this, &ARadiantPlayerController::OnAbilitySix);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &ARadiantPlayerController::ToggleCameraBool);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Started, this, &ARadiantPlayerController::HoldCamera);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Completed, this, &ARadiantPlayerController::ReleaseHoldCamera);
		EnhancedInputComponent->BindAction(AttackMoveAction, ETriggerEvent::Started, this, &ARadiantPlayerController::AttackMove);
	}
}

void ARadiantPlayerController::OnClick(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnUpdateTarget(Value);
}

void ARadiantPlayerController::OnAbilityOne(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityOne(Value);
}

void ARadiantPlayerController::OnAbilityTwo(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityTwo(Value);
}

void ARadiantPlayerController::OnAbilityThree(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityThree(Value);
}

void ARadiantPlayerController::OnAbilityFour(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFour(Value);
}

void ARadiantPlayerController::OnAbilityFive(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFive(Value);
}

void ARadiantPlayerController::OnAbilitySix(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilitySix(Value);
}

void ARadiantPlayerController::ToggleCameraBool(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ToggleCameraBool(Value);
}

void ARadiantPlayerController::HoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->HoldCamera(Value);
}

void ARadiantPlayerController::ReleaseHoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ReleaseHoldCamera(Value);
}

void ARadiantPlayerController::AttackMove(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->AttackMove(Value);
}
