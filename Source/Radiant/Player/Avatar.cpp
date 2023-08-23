// Fill out your copyright notice in the Description page of Project Settings.


#include "Avatar.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "RTPlayerController.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "AI/NavigationSystemBase.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Building/Building.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/AbilityDataAsset.h"
#include "Event/EventBroker.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Abilities/RangedBasicAttack.h"
#include "GAS/Abilities/GAMove.h"
#include "GAS/Abilities/MeleeBasicAttack.h"
#include "GAS/Abilities/RTAbility.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
#include "Items/WorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"
#include "Modes/Base/RTGameState.h"
#include "UI/HeroInfoBar.h"
#include "UI/RTHUD.h"
#include "UI/Client/ClientSubsystem.h"
#include "UI/InGame/InGameStore.h"
#include "Util/UserSettings.h"
#include "Util/Util.h"
#include "Util/Interfaces/Targetable.h"
#include "Util/Managers/ActorManager.h"

// Sets default values
AAvatar::AAvatar()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bCameraLocked = true;

	MainCamera = CreateDefaultSubobject<UCameraComponent>(FName("MainCamera"));
	UnlockedCamera = CreateDefaultSubobject<UCameraComponent>(FName("UnlockedCamera"));
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(FName("SpringArm"));

	SpringArm->SetupAttachment(RootComponent);
	MainCamera->SetupAttachment(SpringArm);
	UnlockedCamera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(FName("HitBox"));
	check(HitBox);
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	OverHeadInfoBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("InfoBar");
	OverHeadInfoBarWidgetComponent->SetupAttachment(RootComponent);
}

void AAvatar::S_ToggleItemMagnet_Implementation()
{
	IsMagnetized = !IsMagnetized;
}

void AAvatar::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);

	if (OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Casting"))))
	{
		bShouldActivateBuffer = true;
	}
	const UGAMove* MoveAbility = Cast<UGAMove>(GameplayAbility);
	const URangedBasicAttack* BasicAttackAbility = Cast<URangedBasicAttack>(GameplayAbility);
	const UMeleeBasicAttack* MeleeBasicAttackAbility = Cast<UMeleeBasicAttack>(GameplayAbility);
	if (MoveAbility || BasicAttackAbility || MeleeBasicAttackAbility)
	{
		return;
	}
	UGameplayStatics::PlaySound2D(GetWorld(), FailedSound, 0.5, 1.5);
}

void AAvatar::CastingTagChanged(FGameplayTag GameplayTag, int I)
{
	if (bShouldActivateBuffer && I == 0)
	{
		AbilitySystemComponent->HandleGameplayEvent(BufferAbility.EventTag, &BufferAbility);
		bShouldActivateBuffer = false;
	}
}

void AAvatar::S_StopMovement_Implementation()
{
	StopMovement();
}

void AAvatar::StopMovement()
{
	GetCharacterMovement()->StopActiveMovement();
}

void AAvatar::SetOverheadBarText(const FString& String)
{
	if (OverHeadInfoBar)
	{
		OverHeadInfoBar->SetOverheadText(String);
	}
}

bool AAvatar::TryPickupItem()
{
	FHitResult HitResult = GetMousePositionInWorld(true);
	if (HitResult.GetActor())
	{
		if (AWorldItem* WorldItem = Cast<AWorldItem>(HitResult.GetActor()))
		{
			S_PickUpItem(WorldItem);
			return true;
		}
	}
	return false;
}

void AAvatar::GiveItem(const FName& ItemName, int32 Amount)
{
	GetRTPS()->S_GiveItem(ItemName, Amount);
}

void AAvatar::LevelUp_Implementation(float GetLevel)
{
	//GetController<ARTPlayerController>()->GetHUD<ARTHUD>()->ShowLevelUpScreen();
}

void AAvatar::GameEnding_Implementation(bool Won)
{
	GetController<ARTPlayerController>()->GetHUD<ARTHUD>()->ShowEndScreen(Won);
	UGameplayStatics::PlaySound2D(GetWorld(), Won ? WinSound : LoseSound);
}


void AAvatar::SetIsDraggingFalse()
{
	bIsDragging = false;
}

void AAvatar::OnDragStatusChanged(bool status)
{
	if (status)
	{
		bIsDragging = true;
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAvatar::SetIsDraggingFalse, 0.01f, false);
	}
}

void AAvatar::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	Destination = GetActorLocation();
	UnlockedCamera->SetWorldRotation(MainCamera->GetComponentRotation());
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->bShowMouseCursor = true;
	}
	OverHeadInfoBar = Cast<UHeroInfoBar>(OverHeadInfoBarWidgetComponent->GetWidget());
	if (OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(1.f);
		OverHeadInfoBar->SetManaPercent(1.f);
		ARTPlayerState* PS = GetRTPS();
		if(PS)
		{
			auto AS = PS->GetAttributeSet();
			if (AS)
			{
				OverHeadInfoBar->SetXPPercent(AS->GetXP() / AS->GetMaxXP());
				OverHeadInfoBar->SetLevel(AS->GetLevel());
			}
		}
	}
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if (PS)
	{
		SetOverheadBarText(PS->GetUsername());
	}
	SetOwnHealthBarColor();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")),
		                                                 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AAvatar::CastingTagChanged);
	}
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AAvatar::ShowStats, 0.3f, true);
	UEventBroker::Get(this)->DragStatusChanged.AddUObject(this, &AAvatar::OnDragStatusChanged);
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass()));
	UEventBroker::Get(this)->GameIsReady.AddUObject(this, &AAvatar::GameReady);
}

void AAvatar::GameReady()
{
	SetOwnHealthBarColor();
}

FVector2D AAvatar::GetMousePosition()
{
	FVector2D MousePosition;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetMousePosition(MousePosition);
	}
	return MousePosition;
}

FHitResult AAvatar::GetMousePositionInWorld(bool bIgnoreSelf) const
{
	FVector2D MousePosition = GetMousePosition();
	FVector WorldPosition;
	FVector WorldDirection;

	if(!IsValid(this) || !GetWorld())
	{
		return FHitResult();
	}
	UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), MousePosition, WorldPosition,
	                                         WorldDirection);

	FHitResult PlayerHitResult;
	FHitResult GroundHitResult;
	FCollisionQueryParams CollisionQueryParams;
	if (bIgnoreSelf)
	{
		CollisionQueryParams.AddIgnoredActor(this);
	}
	GetWorld()->LineTraceSingleByChannel(PlayerHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000,
	                                     ECC_GameTraceChannel1, CollisionQueryParams);
	GetWorld()->LineTraceSingleByChannel(GroundHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000,
	                                     ECC_GameTraceChannel2, CollisionQueryParams);

	return FHitResult(PlayerHitResult.GetActor(), PlayerHitResult.GetComponent(), GroundHitResult.Location,
	                  GroundHitResult.ImpactNormal);
}

void AAvatar::OnUpdateTarget(const FInputActionValue& Value)
{
	FHitResult HitResult = GetMousePositionInWorld(true);
	ITargetable* Targetable = Cast<ITargetable>(HitResult.GetActor());
	ITeamMember* TeamMember = Cast<ITeamMember>(HitResult.GetActor());
	if (Targetable && TeamMember->GetTeamId() != GetTeamId())
	{
		if(Target == HitResult.GetActor())
		{
			return;
		}
		Target = HitResult.GetActor();
		GetPlayerState<ARTPlayerState>()->S_SetTarget(HitResult.GetActor());
	}
	else
	{
		Target = nullptr;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SystemTemplate, HitResult.Location);
	}

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.PathTo"));
	CastAbility(Tag, true);
}

void AAvatar::S_SpawnActorAtMouse_Implementation(const FString& PieceName, const uint32 Amount, const FVector& Location)
{
	if (HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		uint32 Dim = UKismetMathLibrary::Sqrt(Amount);

		for (uint32 x = 0; x < Dim; x++)
		{
			for (uint32 y = 0; y < Dim; y++)
			{
				FVector Offset = FVector(x * 5, y * 5, 100);
				GetWorld()->SpawnActor<AActor>(DebugSpawnableItems[PieceName], Location + Offset, FRotator::ZeroRotator,
				                               SpawnParams);
			}
		}
	}
}

void AAvatar::SpawnActorAtMouse(const FString& PieceName, const uint32 Amount)
{
	if (DebugSpawnableItems.Contains(PieceName))
	{
		S_SpawnActorAtMouse(PieceName, Amount, UUtil::GetMousePosition(this, {}));
	}
	else
	{
		RTPRINTP("Error: Invalid PieceName: %s", *PieceName);
	}
}

void AAvatar::SpawnActorAtSelf(const FString& PieceName, const uint32 Amount)
{
	if (DebugSpawnableItems.Contains(PieceName))
	{
		S_SpawnActorAtMouse(PieceName, Amount, GetActorLocation());
	}
	else
	{
		RTPRINTP("Error: Invalid PieceName: %s", *PieceName);
	}
}

void AAvatar::GiveArchetypeItems(const EClassType Archetype)
{
	if (UDataTable* DataTable = UUtil::GetItemDataTable())
	{
		DataTable->GetRowMap();
		for (auto& Item : DataTable->GetRowMap())
		{
			FItemData* ItemData = reinterpret_cast<FItemData*>(Item.Value);
			if (ItemData->ClassType == Archetype)
			{
				GiveItem(Item.Key, 100);
			}
		}
	}
}

void AAvatar::ClearInventory()
{
	if(UInventoryComponent* InventoryComponent = GetInventory())
	{
		InventoryComponent->S_RemoveAllItems();
	}
}

void AAvatar::S_TryPlaceGridPiece_Implementation(FGridPiece Piece, FName ItemName)
{
	if(GridManager->CheckCanPlace(Piece))
	{
		GetInventory()->S_ItemUsed(ItemName);
		GridManager->PlacePieceAtMouse(Piece);
	}
}

void AAvatar::S_PickUpItem_Implementation(AWorldItem* WorldItem)
{
	if(WorldItem)
	{
		WorldItem->PickUp(this);
	}
}

UInventoryComponent* AAvatar::GetInventory() const
{
	if (GetController<ARTPlayerController>())
	{
		return GetController<ARTPlayerController>()->GetInventory();
	}
	return nullptr;
}

bool AAvatar::CheckShouldAttack()
{
	if(bShouldActivateBuffer)
	{
		bIsAttacking = false;
		return false;
	}

	if(!IsValid(Target))
	{
		bIsAttacking = false;
		Target = nullptr;
		return false;
	}

	if(IKillable* Killable = Cast<IKillable>(Target))
	{
		if(Killable->GetIsDead())
		{
			bIsAttacking = false;
			Target = nullptr;
			return false;
		}
	}

	FVector dir = Target->GetActorLocation() - GetActorLocation();
	bIsAttacking = dir.Size() < AttributeSet->GetAttackRange();

	FGameplayTagContainer OwnedTags;
	if (IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Target))
	{
		Interface->GetAbilitySystemComponent()->GetOwnedGameplayTags(OwnedTags);
	}
	
	BasicAttack();
	
	if(GetIsDead())
	{
		Target = nullptr;
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack")));
		GetAbilitySystemComponent()->CancelAbilities(&TagContainer);
	}

	return true;
}

void AAvatar::CastHotBarAbility(const FGameplayTag& AbilityTag, bool bIgnoreSelf)
{
	if(HasTag("CC.Silenced"))
	{
		return;
	}
	CastAbility(AbilityTag, bIgnoreSelf);
}

void AAvatar::CastAbility(const FGameplayTag& AbilityTag, bool bIgnoreSelf)
{
	if(GetIsDead())
	{
		return;
	}
	
	TArray<AActor*> Actors;
	FGameplayEventData EventData;
	FGameplayAbilityTargetData_SingleTargetHit* MousePosData = new FGameplayAbilityTargetData_SingleTargetHit();

	FHitResult HitResult = GetMousePositionInWorld(bIgnoreSelf);
	MousePosData->HitResult = HitResult;

	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.Add(MousePosData);

	EventData.TargetData = TargetData;
	EventData.EventTag = AbilityTag;
	const FGameplayTag EventTag = AbilityTag;

	BufferAbility = EventData;
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AAvatar::OnAbilityOne(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFirst));
}

void AAvatar::OnAbilityTwo(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarTwo));
}

void AAvatar::OnAbilityThree(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarThree));
}

void AAvatar::OnAbilityFour(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFour));
}

void AAvatar::OnAbilityFive(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFive));
}

void AAvatar::OnAbilitySix(const FInputActionValue& Value)
{
	CastHotBarAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarLast));
}

ARTPlayerState* AAvatar::GetRTPS() const
{
	return GetPlayerState<ARTPlayerState>();
}

void AAvatar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		AttributeSet = PS->GetAttributeSet();
	}
	GiveInitialAbilities();

	if (UActorManager* ActorManager = GetGameInstance()->GetSubsystem<UActorManager>())
	{
		ActorManager->AddPlayer(this);
	}
}

void AAvatar::OnXPChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OverHeadInfoBar)
	{
		if (auto PS = GetPlayerState<ARTPlayerState>())
		{
			float CurrentXP = PS->GetAttributeSet()->GetXP();
			float MaxXP = PS->GetAttributeSet()->GetMaxXP();
			OverHeadInfoBar->SetXPPercent(CurrentXP / MaxXP);
		}
	}
}

void AAvatar::OnLevelChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OverHeadInfoBar)
	{
		OverHeadInfoBar->SetLevel(OnAttributeChangeData.NewValue);
	}
}

void AAvatar::ApplyInitialEffects()
{
	if(HasAuthority())
	{
		for (auto Effect : InitialEffects)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1,
															  AbilitySystemComponent->MakeEffectContext());
		}
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")),
														 EGameplayTagEventType::NewOrRemoved).AddUObject(
			this, &AAvatar::CastingTagChanged);
	}
}

void AAvatar::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		AttributeSet = PS->GetAttributeSet();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).
		                        AddUObject(this, &AAvatar::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetXPAttribute()).AddUObject(
			this, &AAvatar::OnXPChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetLevelAttribute()).
		                        AddUObject(this, &AAvatar::OnLevelChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).
		                        AddUObject(this, &AAvatar::OnManaChanged);
	}

	if (!HasAuthority())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(this, ABuilding::StaticClass(), FoundActors);
		for (AActor* Actor : FoundActors)
		{
			ABuilding* Building = Cast<ABuilding>(Actor);
			if (IsValid(Building))
			{
				Building->SetHealthBarColor();
			}
		}
	}
}

void AAvatar::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (UGameplayStatics::DoesSaveGameExist("UserSettings", 0))
	{
		if (UUserSettings* Save = Cast<UUserSettings>(UGameplayStatics::LoadGameFromSlot("UserSettings", 0)))
		{
			CameraMovementSpeed = Save->CameraSpeed;
		}
	}
}

ETeamId AAvatar::GetTeamId() const
{
	if (GetPlayerState<ARTPlayerState>())
	{
		return GetPlayerState<ARTPlayerState>()->GetTeamId();
	}
	return ETeamId::Neutral;
}


void AAvatar::GiveInitialAbilities()
{
	for(UAbilityDataAsset* AbilityData : GetRTPS()->GetInnateAbilities())
	{	
		FGameplayAbilitySpec AbilitySpec = AbilityData->Ability.GetDefaultObject();
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
}

void AAvatar::SetIsDead(const bool NewIsDead)
{
	Super::SetIsDead(NewIsDead);
	GetRTPS()->SetIsDead(NewIsDead);
	if(NewIsDead)
	{
		M_PlayDeathMontage();
	}
}

void AAvatar::M_PlayDeathMontage_Implementation()
{
	GetCapsuleComponent()->SetCollisionObjectType(ECC_PhysicsBody);
	PlayAnimMontage(DeathMontage);
}

ARTHUD* AAvatar::GetRTHUD()
{
	if (ARTPlayerController* PC = Cast<ARTPlayerController>(GetController()))
	{
		return PC->GetHUD<ARTHUD>();
	}
	return nullptr;
}

void AAvatar::S_CancelAllAbilities_Implementation()
{
	AbilitySystemComponent->CancelAllAbilities();
}

void AAvatar::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.Attribute == AttributeSet->GetHealthAttribute())
	{
		float Percent = AttributeSet->GetHealth() / AttributeSet->GetMaxHealth();
		OverHeadInfoBar->SetHealthPercent(Percent);
	}
}

void AAvatar::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.Attribute == AttributeSet->GetManaAttribute())
	{
		float Percent = AttributeSet->GetMana() / AttributeSet->GetMaxMana();
		OverHeadInfoBar->SetManaPercent(Percent);
	}
}

void AAvatar::SetRotationLock(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
	S_SetRotationLock(RotationLocked, TargetDir);
}

void AAvatar::C_SetRotationLock_Implementation(bool RotationLocked, FVector TargetDir)
{
	SetRotationLock(RotationLocked, TargetDir);
}

void AAvatar::ResetDestination()
{
	Destination = GetActorLocation();
}

void AAvatar::SetDestination(FVector NewDestination)
{
	Destination = NewDestination;
}

void AAvatar::M_SetInfoBarVisibility_Implementation(bool bVisible)
{
	OverHeadInfoBarWidgetComponent->SetVisibility(bVisible);
}

void AAvatar::SetOwnHealthBarColor()
{
	if (GetPlayerState<ARTPlayerState>())
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ARTPlayerState* LocalPS = Cast<ARTPlayerState>(PC->GetPlayerState<ARTPlayerState>());
		if (!LocalPS)
		{
			return;
		}
		ETeamId Id = GetPlayerState<ARTPlayerState>()->GetTeamId();
		FLinearColor Color;
		if (LocalPS)
		{
			if (Id == LocalPS->GetTeamId())
			{
				Color = FLinearColor::Green;
			}
			else
			{
				Color = FLinearColor::Red;
			}
		}
		SetHealthColor(Color);
	}
}

void AAvatar::SetHealthColor(const FLinearColor Color)
{
	if (OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthColor(Color);
	}
}

void AAvatar::SetAllHealthBarColors()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		if (GetPlayerState<ARTPlayerState>())
		{
			ETeamId Id = GetPlayerState<ARTPlayerState>()->GetTeamId();
			TArray<APlayerState*> States = GetWorld()->GetGameState<ARTGameState>()->PlayerArray;
			if (States.Num() > 0)
			{
				for (auto State : States)
				{
					if (auto s = Cast<ARTPlayerState>(State))
					{
						ETeamId OtherId = s->GetTeamId();
						if (Id == OtherId)
						{
							State->GetPawn<AAvatar>()->SetHealthColor(FLinearColor::Green);
						}
						else
						{
							State->GetPawn<AAvatar>()->SetHealthColor(FLinearColor::Red);
						}
					}
				}
			}
		}
	}
}

FVector AAvatar::GetHalfHeightVector()
{
	return FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
}

void AAvatar::BasicAttack()
{
	if (IsLocallyControlled() && bIsAttacking)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
		FGameplayEventData EventData;
		GetAbilitySystemComponent()->HandleGameplayEvent(Tag, &EventData);
	}
}

bool AAvatar::HasTag(FString Tag)
{
	FGameplayTagContainer TagContainer;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->GetOwnedGameplayTags(TagContainer);
		return TagContainer.HasTag(FGameplayTag::RequestGameplayTag(FName(Tag)));
	}
	return false;
}

void AAvatar::ToggleCameraBool(const FInputActionValue& Value)
{
	bCameraLocked = !bCameraLocked;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(bCameraLocked ? EMouseLockMode::DoNotLock : EMouseLockMode::LockAlways);
	GetController<APlayerController>()->SetInputMode(InputMode);
	UnlockedCamera->SetWorldLocation(MainCamera->GetComponentLocation());
	UnlockedCamera->SetActive(!bCameraLocked);
	MainCamera->SetActive(bCameraLocked);
}

void AAvatar::HoldCamera(const FInputActionValue& Value)
{
	if (!bCameraLocked)
	{
		MainCamera->SetActive(true);
		UnlockedCamera->SetActive(false);
		bCameraHeld = true;
	}
}

void AAvatar::ReleaseHoldCamera(const FInputActionValue& InputActionValue)
{
	if (!bCameraLocked)
	{
		MainCamera->SetActive(false);
		UnlockedCamera->SetActive(true);
		UnlockedCamera->SetWorldLocation(MainCamera->GetComponentLocation());
		bCameraHeld = false;
	}
}

void AAvatar::AttackMove(const FInputActionValue& Value)
{
	const FHitResult HitResult = GetMousePositionInWorld();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, AttackMoveSystemTemplate, HitResult.Location);

	TArray<AActor*> OverlappingActors;
	UKismetSystemLibrary::SphereOverlapActors(this, HitResult.Location, 2000.f, TArray<TEnumAsByte<EObjectTypeQuery>>(),
	                                          StaticClass(), {this}, OverlappingActors);

	for (auto Actor : OverlappingActors)
	{
		if (auto Avatar = Cast<AAvatar>(Actor))
		{
			if (Avatar->GetPlayerState<ARTPlayerState>()->GetTeamId() != GetPlayerState<ARTPlayerState>()->GetTeamId())
			{
				float DistToNew = FVector::Dist(HitResult.Location, Avatar->GetActorLocation());
				float DistToOld = 0;
				if (Target)
				{
					DistToOld = FVector::Dist(HitResult.Location, Target->GetActorLocation());
				}
				if (DistToNew < DistToOld || !Target)
				{
					Target = Avatar;
				}
			}
		}
	}

	if (OverlappingActors.Num() == 0)
	{
		UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), 2000.f,
		                                          TArray<TEnumAsByte<EObjectTypeQuery>>(),
		                                          StaticClass(), {this}, OverlappingActors);

		for (auto Actor : OverlappingActors)
		{
			if (auto Avatar = Cast<AAvatar>(Actor))
			{
				if (Avatar->GetPlayerState<ARTPlayerState>()->GetTeamId() != GetPlayerState<ARTPlayerState>()->
					GetTeamId())
				{
					float DistToNew = FVector::Dist(GetActorLocation(), Avatar->GetActorLocation());
					float DistToOld = 0;
					if (Target)
					{
						DistToOld = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
					}
					if (DistToNew < DistToOld || !Target)
					{
						Target = Avatar;
					}
				}
			}
		}
	}

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.PathTo"));
	CastAbility(Tag);
}

void AAvatar::MoveCamera(FVector Dir)
{
	if (!bCameraLocked)
	{
		FVector CameraMove = FVector::ZeroVector;
		CameraMove += Dir * CameraMovementSpeed * GetWorld()->DeltaTimeSeconds;
		CameraMove = CameraMove.RotateAngleAxis(UnlockedCamera->GetComponentRotation().Yaw + 90, FVector::UpVector);
		UnlockedCamera->SetWorldLocation(UnlockedCamera->GetComponentLocation() + CameraMove);
	}
}

void AAvatar::Demolish()
{
	FHitResult HitResult = GetMousePositionInWorld();
	if (ABuilding* Building = Cast<ABuilding>(HitResult.GetActor()))
	{
		S_Demolish(GetRTPS()->GetTeamId(), Building);
	}
}

void AAvatar::S_Demolish_Implementation(ETeamId PlayerTeamId, ABuilding* Building)
{
	if (HasAuthority())
	{
		if (IsValid(Building))
		{
			if (PlayerTeamId == Building->GetTeamId())
			{
				Building->DestroyBuilding();
			}
		}
	}
}

void AAvatar::HandleCamera(float DeltaSeconds)
{
	if (!bCameraLocked && !bIsDragging)
	{
		FVector2D MousePosition = GetMousePosition();
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector CameraMove = FVector::ZeroVector;
		if (MousePosition.X <= CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * -CameraMovementSpeed;
		}
		else if (MousePosition.X >= ViewportSize.X - CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * CameraMovementSpeed;
		}

		if (MousePosition.Y <= CameraMovementThreshold)
		{
			CameraMove += FVector::RightVector * -CameraMovementSpeed;
		}
		else if (MousePosition.Y >= ViewportSize.Y - CameraMovementThreshold)
		{
			CameraMove += FVector::RightVector * CameraMovementSpeed;
		}
		CameraMove = CameraMove.RotateAngleAxis(UnlockedCamera->GetComponentRotation().Yaw + 90, FVector::UpVector);
		UnlockedCamera->SetWorldLocation(UnlockedCamera->GetComponentLocation() + CameraMove * DeltaSeconds);
	}
}

void AAvatar::Restart()
{
	Super::Restart();
}

void AAvatar::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AAvatar::PostLoad()
{
	Super::PostLoad();
}

void AAvatar::S_SetRotationLock_Implementation(bool RotationLocked, FVector TargetDir)
{
	bRotationLocked = RotationLocked;
	TargetDirection = TargetDir;
	GetCharacterMovement()->bOrientRotationToMovement = !RotationLocked;
	bUseControllerRotationYaw = RotationLocked;
}

void AAvatar::ShowStats()
{
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ARTPlayerController* PC = GetController<ARTPlayerController>();
		if (PC)
		{
			PC->GetHUD<ARTHUD>()->SetFPS(FPS);
			if (GetPlayerState())
			{
				PC->GetHUD<ARTHUD>()->SetMS(GetPlayerState()->GetPingInMilliseconds());
			}
		}
	}
}

// Called every frame
void AAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FPS = 1.0 / DeltaTime;
	HandleCamera(DeltaTime);

	if (IsLocallyControlled())
	{
		if (bRotationLocked)
		{
			FRotator CurrentRotation = GetCapsuleComponent()->GetComponentRotation();
			FRotator Rotation = FMath::Lerp(CurrentRotation, TargetDirection.Rotation(), RotationSpeed * DeltaTime);
			Rotation.Pitch = CurrentRotation.Pitch;
			Rotation.Roll = CurrentRotation.Roll;
			GetController()->SetControlRotation(Rotation);
		}
	}
	if (HasTag("States.Movement.Stopped") || HasTag("States.Dead"))
	{
		StopMovement();
	}
}

void AAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}