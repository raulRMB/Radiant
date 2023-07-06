// Copyright Radiant Studios

#include "RTPlayerController.h"

#include "Avatar.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InventoryComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Modes/Base/RTGameMode.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Client/ClientSubsystem.h"
#include "UI/RTHUD.h"
#include "Util/UserSettings.h"
#include "Util/Util.h"

void ARTPlayerController::PlayerLoaded_Implementation()
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->PlayerLoaded();
}

void ARTPlayerController::S_SetPlayerStart_Implementation(ARTPlayerStart* NewPlayerStart)
{
	PlayerStart = NewPlayerStart;	
}

void ARTPlayerController::Connected_Implementation()
{
	if(UClientSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UClientSubsystem>())
	{
		SetQueueName(Subsystem->QueueName);
	}
}

void ARTPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARTPlayerController, PlayerStart)
}

void ARTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
		LoadUserSettings(Subsystem);
	}

	if(InventoryComponent)
	{
		InventoryComponent->InitInventory(ItemDataTable);
	}
}

FVector ARTPlayerController::GetCarrierLocation() const
{
	if(GetPawn())
	{
		return GetPawn()->GetActorLocation();
	}
	return FVector::ZeroVector;
}

void ARTPlayerController::DropItem(const FName& ItemName)
{
	ICarrier::DropItem(ItemName);

	// TODO: REMOVE ABILITY
}

void ARTPlayerController::LoadUserSettings(UEnhancedInputLocalPlayerSubsystem* Subsystem)
{
	if(UGameplayStatics::DoesSaveGameExist("UserSettings", 0))
	{
		UUserSettings* Save = Cast<UUserSettings>(UGameplayStatics::LoadGameFromSlot("UserSettings", 0));
		for(FEnhancedActionKeyMapping& Mapping : Save->Mappings)
		{
			Subsystem->AddPlayerMappedKey(Mapping.PlayerMappableOptions.Name, Mapping.Key);
		}
	}
}

void ARTPlayerController::SaveUserSettingsDelay()
{
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &ARTPlayerController::SaveUserSettings, 0.2f, false);
}

void ARTPlayerController::SetQueueName_Implementation(const FString& QueueName)
{
	ARTGameMode* GM = GetWorld()->GetAuthGameMode<ARTGameMode>();
	GM->SetTeamSize(QueueName);
}

FGameplayTag ARTPlayerController::GetAbilityTrigger(const uint32 i) const
{
	return GetInventory()->GetAbilityTrigger(i);
}

ARTPlayerController::ARTPlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	if(InventoryComponent)
	{
		InventoryComponent->SetIsReplicated(true);
	}
}

void ARTPlayerController::SaveUserSettings()
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UUserSettings* Save;
		if(UGameplayStatics::DoesSaveGameExist("UserSettings", 0)) {
			Save = Cast<UUserSettings>(UGameplayStatics::LoadGameFromSlot("UserSettings", 0));
		}
		else
		{
			Save = Cast<UUserSettings>(UGameplayStatics::CreateSaveGameObject(UUserSettings::StaticClass()));
		}
		Save->Mappings = Subsystem->GetAllPlayerMappableActionKeyMappings();
		Save->CameraSpeed = GetPawn<AAvatar>()->CameraMovementSpeed;
		UGameplayStatics::SaveGameToSlot(Save, "UserSettings", 0);
	}
}

void ARTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &ARTPlayerController::OnClick);
		EnhancedInputComponent->BindAction(AbilityOneAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityOne);
		EnhancedInputComponent->BindAction(AbilityTwoAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityTwo);
		EnhancedInputComponent->BindAction(AbilityThreeAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityThree);
		EnhancedInputComponent->BindAction(AbilityFourAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityFour);
		EnhancedInputComponent->BindAction(AbilityFiveAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilityFive);
		EnhancedInputComponent->BindAction(AbilitySixAction, ETriggerEvent::Started, this, &ARTPlayerController::OnAbilitySix);
		EnhancedInputComponent->BindAction(CameraToggleAction, ETriggerEvent::Started, this, &ARTPlayerController::ToggleCameraBool);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Started, this, &ARTPlayerController::HoldCamera);
		EnhancedInputComponent->BindAction(CameraHoldAction, ETriggerEvent::Completed, this, &ARTPlayerController::ReleaseHoldCamera);
		EnhancedInputComponent->BindAction(AttackMoveAction, ETriggerEvent::Started, this, &ARTPlayerController::AttackMove);
		EnhancedInputComponent->BindAction(CameraMoveAction, ETriggerEvent::Triggered, this, &ARTPlayerController::CameraMove);
		EnhancedInputComponent->BindAction(ToggleStoreAction, ETriggerEvent::Triggered, this, &ARTPlayerController::ToggleStore);
		EnhancedInputComponent->BindAction(EscapeAction, ETriggerEvent::Triggered, this, &ARTPlayerController::Escape);
		EnhancedInputComponent->BindAction(AcceptOrderAction, ETriggerEvent::Triggered, this, &ARTPlayerController::AcceptOrder);
	}
}

void ARTPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (GetLocalRole() < ROLE_Authority)
	{
		UPathFollowingComponent* PathFollowingComp = FindComponentByClass<UPathFollowingComponent>();
		if (PathFollowingComp)
		{
			PathFollowingComp->UpdateCachedComponents();
		}
	}
}

void ARTPlayerController::OnClick(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnUpdateTarget(Value);
}

void ARTPlayerController::OnAbilityOne(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityOne(Value);
}

void ARTPlayerController::OnAbilityTwo(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityTwo(Value);
}

void ARTPlayerController::OnAbilityThree(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityThree(Value);
}

void ARTPlayerController::OnAbilityFour(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFour(Value);
}

void ARTPlayerController::OnAbilityFive(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilityFive(Value);
}

void ARTPlayerController::OnAbilitySix(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->OnAbilitySix(Value);
}

void ARTPlayerController::ToggleCameraBool(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ToggleCameraBool(Value);
}

void ARTPlayerController::HoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->HoldCamera(Value);
}

void ARTPlayerController::ReleaseHoldCamera(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->ReleaseHoldCamera(Value);
}

void ARTPlayerController::AttackMove(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->AttackMove(Value);
}

void ARTPlayerController::CameraMove(const FInputActionValue& Value)
{
	GetPawn<AAvatar>()->MoveCamera(Value.Get<FVector>());
}

void ARTPlayerController::ToggleStore(const FInputActionValue& Value)
{
	GetHUD<ARTHUD>()->ToggleStore();
}

void ARTPlayerController::Escape(const FInputActionValue& Value)
{
	GetHUD<ARTHUD>()->Escape();
}

void ARTPlayerController::AcceptOrder(const FInputActionValue& Value)
{
	OrderAccepted.Broadcast();
}
