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
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Abilities/RTAbility.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/RTAvatarAttributeSet.h"
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

void AAvatar::GameReadyUnicast_Implementation()
{
	auto SS = GetGameInstance()->GetSubsystem<UClientSubsystem>();
	if(SS)
	{
		FString Username = GetGameInstance()->GetSubsystem<UClientSubsystem>()->Username;
		if(!Username.IsEmpty())
		{
			GetPlayerState<ARTPlayerState>()->SetUsername(Username);
		}
	}
	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
	AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAvatar::CastingTagChanged);
	GetController<ARTPlayerController>()->GetHUD<ARTHUD>()->HideLoadScreen();
	GetController<ARTPlayerController>()->GetHUD<ARTHUD>()->BindUIItems();
}

void AAvatar::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	FGameplayTagContainer OwnedTags;
	AbilitySystemComponent->GetOwnedGameplayTags(OwnedTags);
	if (OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Casting"))))
	{
		bShouldActivateBuffer = true;
	}
	else if(!GameplayAbility->GetName().Contains("BasicAttack"))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), FailedSound, 0.5, 1.5); 
	}
}

void AAvatar::CastingTagChanged(FGameplayTag GameplayTag, int I)
{
	if(bShouldActivateBuffer && I == 0)
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
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetOverheadText(String);
	}
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


void AAvatar::BeginPlay()
{
	Super::BeginPlay();

	Destination = GetActorLocation();
	UnlockedCamera->SetWorldRotation(MainCamera->GetComponentRotation());
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->bShowMouseCursor = true;
	}
	OverHeadInfoBar = Cast<UHeroInfoBar>(OverHeadInfoBarWidgetComponent->GetWidget());
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthPercent(1.f);		
		OverHeadInfoBar->SetManaPercent(1.f);
		auto PS = GetRTPlayerState();
		if(PS)
		{
			auto AS = PS->GetAttributeSetBase();
			if(AS)
			{
				OverHeadInfoBar->SetXPPercent(AS->GetXP() / AS->GetMaxXP());
				OverHeadInfoBar->SetLevel(AS->GetLevel());
			}
		}
	}
	if(!HasAuthority())
	{
		auto PC = Cast<ARTPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		PC->PlayerLoaded();
	}
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		SetOverheadBarText(PS->GetUsername());
	}
	SetOwnHealthBarColor();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &AAvatar::OnAbilityFailed);
		AbilitySystemComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName("States.Casting")), EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAvatar::CastingTagChanged);
	}
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AAvatar::ShowStats, 0.3f, true);
	
	// SetHUDIcons();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(this, AGridManager::StaticClass()));
}

void AAvatar::GameReady_Implementation()
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

FHitResult AAvatar::GetMousePositionInWorld() const
{
	FVector2D MousePosition = GetMousePosition();
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), MousePosition, WorldPosition, WorldDirection);

	FHitResult PlayerHitResult;
	FHitResult GroundHitResult;
	FCollisionQueryParams CollisionQueryParams;
	GetWorld()->LineTraceSingleByChannel(PlayerHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel1, CollisionQueryParams);
	GetWorld()->LineTraceSingleByChannel(GroundHitResult, WorldPosition, WorldPosition + WorldDirection * 1000000, ECC_GameTraceChannel2, CollisionQueryParams);
	
	return FHitResult(PlayerHitResult.GetActor(), PlayerHitResult.GetComponent(), GroundHitResult.Location, GroundHitResult.ImpactNormal);
}

void AAvatar::OnUpdateTarget(const FInputActionValue& Value)
{
	FHitResult HitResult = GetMousePositionInWorld();
	ITargetable* Targetable = Cast<ITargetable>(HitResult.GetActor());
	ITeamMember* TeamMember = Cast<ITeamMember>(HitResult.GetActor());
	if(Targetable && TeamMember->GetTeamId() != GetPlayerState<ARTPlayerState>()->GetTeamId())
	{
		Target = HitResult.GetActor();
		GetPlayerState<ARTPlayerState>()->S_SetTarget(HitResult.GetActor());
	}
	else
	{
		Target = nullptr;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, SystemTemplate, HitResult.Location);
	}
	if(!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Uncancellable"))))
	{
		S_CancelAllAbilities();
	}

	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.PathTo"));
	CastAbility(Tag);
}

void AAvatar::S_SpawnActorAtMouse_Implementation(const FString& PieceName, const uint32 Amount, const FVector& Location)
{
	if(HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		uint32 Dim = UKismetMathLibrary::Sqrt(Amount);
		
		for(uint32 x = 0; x < Dim; x++)
		{
			for(uint32 y = 0; y < Dim; y++)
			{
				FVector Offset = FVector(x * 5, y * 5, 100);
				GetWorld()->SpawnActor<AActor>(DebugSpawnableItems[PieceName], Location + Offset, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}

void AAvatar::SpawnActorAtMouse(const FString& PieceName, const uint32 Amount)
{
	if(DebugSpawnableItems.Contains(PieceName))
	{
		S_SpawnActorAtMouse(PieceName, Amount, UUtil::GetMousePosition(this, {}));
	}
	else
	{
		RTPRINTP("Error: Invalid PieceName: %s", *PieceName);
	}
}

UInventoryComponent* AAvatar::GetInventory() const
{
	if(GetController<ARTPlayerController>())
	{
		return GetController<ARTPlayerController>()->GetInventory();
	}
	return nullptr;
}

bool AAvatar::CheckShouldAttack()
{
	if(!Target || bShouldActivateBuffer)
	{
		bIsAttacking = false;
		return false;
	}
	
	FVector dir = Target->GetActorLocation() - GetActorLocation();
	bIsAttacking = dir.Size() < AttackRange;

	FGameplayTagContainer OwnedTags;
	if(IAbilitySystemInterface* Interface = Cast<IAbilitySystemInterface>(Target))
	{
		Interface->GetAbilitySystemComponent()->GetOwnedGameplayTags(OwnedTags);
	}
	
	if(OwnedTags.HasTag(FGameplayTag::RequestGameplayTag(FName("States.Dead"))))
	{
		Target = nullptr;
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack")));
		GetAbilitySystemComponent()->CancelAbilities(&TagContainer);
	}
	
	BasicAttack();

	return true;
}

void AAvatar::CastAbility(const FGameplayTag& AbilityTag)
{
	TArray<AActor*> Actors;
	FGameplayEventData EventData;
	FGameplayAbilityTargetData_SingleTargetHit* MousePosData = new FGameplayAbilityTargetData_SingleTargetHit();

	FHitResult HitResult = GetMousePositionInWorld();
	MousePosData->HitResult = HitResult;
	
	FGameplayAbilityTargetDataHandle TargetData;
	TargetData.Add(MousePosData);
	
	EventData.TargetData = TargetData;
	EventData.EventTag = AbilityTag;
	const FGameplayTag EventTag = AbilityTag;

	BufferAbility = EventData;
	AbilitySystemComponent->HandleGameplayEvent(EventTag, &EventData);
}

void AAvatar::S_PlaceGridPiece_Implementation(FGridPiece Piece)
{
	GridManager->PlacePieceAtMouse(Piece);	
}

void AAvatar::OnAbilityOne(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFirst));
}

void AAvatar::OnAbilityTwo(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarTwo));
}

void AAvatar::OnAbilityThree(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarThree));
}

void AAvatar::OnAbilityFour(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFour));
}

void AAvatar::OnAbilityFive(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarFive));
}

void AAvatar::OnAbilitySix(const FInputActionValue& Value)
{
	CastAbility(GetRTHUD()->GetAbilityTrigger(EItemSlotID::HotBarLast));
}

void AAvatar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		AttributeSetBase = PS->GetAttributeSetBase();

	}
	GiveInitialAbilities();

	if(UActorManager* ActorManager = GetGameInstance()->GetSubsystem<UActorManager>())
	{
		ActorManager->AddPlayer(this);
	}
}

void AAvatar::OnXPChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if(OverHeadInfoBar)
	{
		if(auto PS = GetPlayerState<ARTPlayerState>())
		{
			float CurrentXP = PS->GetAttributeSetBase()->GetXP();
			float MaxXP = PS->GetAttributeSetBase()->GetMaxXP();
			OverHeadInfoBar->SetXPPercent(CurrentXP / MaxXP);
		}
	}
}

void AAvatar::OnLevelChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetLevel(OnAttributeChangeData.NewValue);
	}
}

void AAvatar::ApplyInitialEffects()
{
	for(auto Effect : InitialEffects)
	{
		UGameplayEffect *EffectInstance = NewObject<UGameplayEffect>(this, Effect);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(EffectInstance, 1, AbilitySystemComponent->MakeEffectContext());
	}
}

void AAvatar::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	ARTPlayerState* PS = GetPlayerState<ARTPlayerState>();
	if(PS)
	{
		AbilitySystemComponent = Cast<URTAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		AttributeSetBase = PS->GetAttributeSetBase();
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AAvatar::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetXPAttribute()).AddUObject(this, &AAvatar::OnXPChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetLevelAttribute()).AddUObject(this, &AAvatar::OnLevelChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetManaAttribute()).AddUObject(this, &AAvatar::OnManaChanged);
	}


	if(!HasAuthority())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(this, ABuilding::StaticClass(), FoundActors);
		for(AActor* Actor : FoundActors)
		{
			ABuilding* Building = Cast<ABuilding>(Actor);
			if(IsValid(Building))
			{
				Building->SetHealthBarColor();
			}
		}
	}
}

void AAvatar::OnRep_Controller()
{
	Super::OnRep_Controller();

	if(UGameplayStatics::DoesSaveGameExist("UserSettings", 0))
	{
		if(UUserSettings* Save = Cast<UUserSettings>(UGameplayStatics::LoadGameFromSlot("UserSettings", 0)))
		{
			CameraMovementSpeed = Save->CameraSpeed;
		}
	}
}

void AAvatar::SetHUDIcons(const TMap<EItemSlotID, FItemSlotInfo>& AbilityMap)
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		if(ARTPlayerController* PC = Cast<ARTPlayerController>(GetController()))
		{
			PC->GetHUD<ARTHUD>()->UpdateAbilities(AbilityMap);
		}
	}
}

ETeamId AAvatar::GetTeamId() const
{
	if(GetPlayerState<ARTPlayerState>())
	{
		return GetPlayerState<ARTPlayerState>()->GetTeamId();
	}
	return ETeamId::Neutral;
}


void AAvatar::GiveInitialAbilities()
{
	for(auto AbilityData : GetRTPlayerState()->GetInnateAbilities())
	{	
		FGameplayAbilitySpec AbilitySpec = AbilityData->Ability.GetDefaultObject();
		AbilitySystemComponent->GiveAbility(AbilitySpec);
	}
	GiveDeathAbilities();
}

ARTHUD* AAvatar::GetRTHUD()
{
	if(ARTPlayerController* PC = Cast<ARTPlayerController>(GetController()))
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
	if(Data.Attribute == AttributeSetBase->GetHealthAttribute())
	{
		float Percent = AttributeSetBase->GetHealth() / AttributeSetBase->GetMaxHealth();
		OverHeadInfoBar->SetHealthPercent(Percent);
	}
}

void AAvatar::OnManaChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == AttributeSetBase->GetManaAttribute())
	{
		float Percent = AttributeSetBase->GetMana() / AttributeSetBase->GetMaxMana();
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
	if(GetPlayerState<ARTPlayerState>())
	{
		APlayerController *PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ARTPlayerState* LocalPS = Cast<ARTPlayerState>(PC->GetPlayerState<ARTPlayerState>());
		if(!LocalPS)
		{
			return;
		}
		ETeamId Id = GetPlayerState<ARTPlayerState>()->GetTeamId();
		FLinearColor Color;
		if(LocalPS)
		{
			if(Id == LocalPS->GetTeamId())
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
	if(OverHeadInfoBar)
	{
		OverHeadInfoBar->SetHealthColor(Color);
	}
}

void AAvatar::SetAllHealthBarColors()
{
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		if(GetPlayerState<ARTPlayerState>())
		{
			ETeamId Id = GetPlayerState<ARTPlayerState>()->GetTeamId();
			TArray<APlayerState*> States = GetWorld()->GetGameState<ARTGameState>()->PlayerArray;
			if(States.Num() > 0)
			{
				for(auto State : States)
				{
					if(auto s = Cast<ARTPlayerState>(State))
					{
						ETeamId OtherId = s->GetTeamId();
						if(Id == OtherId)
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
	if(IsLocallyControlled() && bIsAttacking)
	{
		FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.BasicAttack"));
		FGameplayEventData EventData;		
		GetAbilitySystemComponent()->HandleGameplayEvent(Tag, &EventData);
	}
}

bool AAvatar::HasTag(FString Tag)
{
	FGameplayTagContainer TagContainer;
	if(AbilitySystemComponent)
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
	if(!bCameraLocked)
	{
		MainCamera->SetActive(true);
		UnlockedCamera->SetActive(false);
		bCameraHeld = true;
	}
}

void AAvatar::ReleaseHoldCamera(const FInputActionValue& InputActionValue)
{
	if(!bCameraLocked)
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
	UKismetSystemLibrary::SphereOverlapActors(this, HitResult.Location, 2000.f, TArray<TEnumAsByte<EObjectTypeQuery>>(), AAvatar::StaticClass(), {this}, OverlappingActors);
	
	for(auto Actor : OverlappingActors)
	{
		if(auto Avatar = Cast<AAvatar>(Actor))
		{
			if(Avatar->GetPlayerState<ARTPlayerState>()->GetTeamId() != GetPlayerState<ARTPlayerState>()->GetTeamId())
			{
				float DistToNew = FVector::Dist(HitResult.Location, Avatar->GetActorLocation());
				float DistToOld = 0;
				if(Target)
				{
					DistToOld = FVector::Dist(HitResult.Location, Target->GetActorLocation());
				}
				if(DistToNew < DistToOld || !Target)
				{
					Target = Avatar;
				}
			}
		}
	}

	if(OverlappingActors.Num() == 0)
	{
		UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), 2000.f, TArray<TEnumAsByte<EObjectTypeQuery>>(), AAvatar::StaticClass(), {this}, OverlappingActors);
	
		for(auto Actor : OverlappingActors)
		{
			if(auto Avatar = Cast<AAvatar>(Actor))
			{
				if(Avatar->GetPlayerState<ARTPlayerState>()->GetTeamId() != GetPlayerState<ARTPlayerState>()->GetTeamId())
				{
					float DistToNew = FVector::Dist(GetActorLocation(), Avatar->GetActorLocation());
					float DistToOld = 0;
					if(Target)
					{
						DistToOld = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
					}
					if(DistToNew < DistToOld || !Target)
					{
						Target = Avatar;
					}
				}
			}
		}
	}

	CheckShouldAttack();
	
	FGameplayTag Tag = FGameplayTag::RequestGameplayTag(FName("Ability.PathTo"));
	CastAbility(Tag);
}

void AAvatar::MoveCamera(FVector Dir)
{
	if(!bCameraLocked)
	{
		FVector CameraMove = FVector::ZeroVector;
		CameraMove += Dir * CameraMovementSpeed * GetWorld()->DeltaTimeSeconds;
		CameraMove = CameraMove.RotateAngleAxis(UnlockedCamera->GetComponentRotation().Yaw + 90, FVector::UpVector);
		UnlockedCamera->SetWorldLocation(UnlockedCamera->GetComponentLocation() + CameraMove);
	}
}

void AAvatar::HandleCamera(float DeltaSeconds)
{
	if(!bCameraLocked)
	{
		FVector2D MousePosition = GetMousePosition();
		FVector2D ViewportSize;
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->GetViewportSize(ViewportSize);
		}

		FVector CameraMove = FVector::ZeroVector;
		if(MousePosition.X <= CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * -CameraMovementSpeed;
		} else if(MousePosition.X >= ViewportSize.X - CameraMovementThreshold)
		{
			CameraMove += FVector::ForwardVector * CameraMovementSpeed;
		}

		if(MousePosition.Y <= CameraMovementThreshold)
		{
			CameraMove += FVector::RightVector * -CameraMovementSpeed;
		} else if(MousePosition.Y >= ViewportSize.Y - CameraMovementThreshold)
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
	if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		ARTPlayerController* PC = GetController<ARTPlayerController>();
		if(PC)
		{
			PC->GetHUD<ARTHUD>()->SetFPS(FPS);
			if(GetPlayerState())
				PC->GetHUD<ARTHUD>()->SetMS(GetPlayerState()->GetPingInMilliseconds());
		}
	}
}

// Called every frame
void AAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	FPS = 1.0/DeltaTime;
	HandleCamera(DeltaTime);

	if(IsLocallyControlled())
	{
		if(bRotationLocked)
		{
			FRotator CurrentRotation = GetCapsuleComponent()->GetComponentRotation();
			FRotator Rotation = FMath::Lerp(CurrentRotation, TargetDirection.Rotation(), RotationSpeed * DeltaTime);
			Rotation.Pitch = CurrentRotation.Pitch;
			Rotation.Roll = CurrentRotation.Roll;
			GetController()->SetControlRotation(Rotation);
		}
	}
	if(HasTag("States.Movement.Stopped") || HasTag("States.Dead"))
		StopMovement();
}

void AAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

