#include "Building.h"

#include "GameplayEffectExtension.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "Items/WorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"
#include "UI/AIInfoBar.h"
#include "Util/Util.h"
#include "Util/Managers/ActorManager.h"
#include "Util/Managers/Grid/TeamGridManager.h"

ABuilding::ABuilding()
{
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Box = CreateDefaultSubobject<UBoxComponent>("Box");
	Box->SetupAttachment(RootComponent);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionObjectType(ECC_Pawn);
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_CursorTarget, ECollisionResponse::ECR_Block);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
	Box->SetCollisionResponseToChannel(ECC_Artillery, ECollisionResponse::ECR_Overlap);
	Box->SetCollisionResponseToChannel(ECC_Vision, ECollisionResponse::ECR_Overlap);
	
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<URTBaseAttributeSet>("AttributeSet");
	AttributeSet->InitHealth(MaxHealth);
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitLevel(1.f);
	AttributeSet->InitArmor(20.f);
	AttributeSet->InitAttackDamage(10.f);
	AttributeSet->InitAttackSpeed(1.f);
	AttributeSet->InitMovementSpeed(0.f);
	AttributeSet->InitAttackRange(100.f);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetupAttachment(RootComponent);
		
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;

	NetUpdateFrequency = 10.f;
}

UStaticMeshComponent* ABuilding::GetMesh() const
{
	return Mesh;
}

bool ABuilding::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	bool bIsVisible = false;
	
	if(const ARTPlayerController* PC = Cast<ARTPlayerController>(RealViewer))
	{
		if(PC->GetTeamId() == GetTeamId())
		{
			bIsVisible = true;
		}
		else
		{
			bIsVisible = IsVisibleForTeam(PC->GetTeamId());
		}
		
		if(bIsVisible)
		{
			if(ActorManager)
			{
				if(ATeamGridManager* TGM = ActorManager->GetTeamGridManager(PC->GetTeamId()))
				{
					// TGM->PieceChanged(this);
				}
			}

		}
	}
	
	return bIsVisible;
}

bool ABuilding::IsVisibleForTeam(const ETeamId TargetTeamId) const
{
	if(TargetTeamId == GetTeamId())
	{
		return true;
	}
	
	ATeamGridManager* TeamGridManager = ActorManager ? ActorManager->GetTeamGridManager(TargetTeamId) : nullptr;
	return TeamGridManager ? TeamGridManager->IsTargetInVision(this) : false;
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	ActorManager = GetGameInstance()->GetSubsystem<UActorManager>();
	
	GiveInitialAbilities();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(
		this, &ABuilding::OnHealthChanged);
}


void ABuilding::GiveInitialAbilities()
{
	if (HasAuthority())
	{
		for (auto Ability : Abilities)
		{
			AbilitySystemComponent->GiveAbility(Ability);
		}
	}
}

void ABuilding::S_SetTeamId_Implementation(ETeamId NewTeamId)
{
	TeamId = NewTeamId;
}

void ABuilding::S_Demolish_Implementation()
{
	if (HasAuthority())
	{
		DestroyBuilding();
	}
}

void ABuilding::InitGridValues(AGridManager* NewGridManager, const FGridPiece& NewGridPiece)
{
	GridManager = NewGridManager;
	GridPiece = NewGridPiece;
}

void ABuilding::DestroyBuilding()
{
	if(GridManager)
	{
		GridManager->ClearPiece(this);
		OnDeathNotify.Broadcast(this);
	}
	Destroy();
}

void ABuilding::M_ShowInfoBar_Implementation(bool bCond, float HealthPercent)
{
	if(!bCond)
	{
		return;
	}
	
	if(!HasAuthority())
	{
		if(!InfoBarWidgetComponent)
		{			
			InfoBarWidgetComponent = Cast<UWidgetComponent>(AddComponentByClass(InfoBarActorComponent, false, FTransform(WidgetLocation), false));
			if(InfoBarWidgetComponent)
			{
				InfoBarWidgetComponent->RegisterComponent();
				InfoBarWidgetComponent->SetWidgetClass(InfoBarWidgetClass);
				InfoBarWidgetComponent->InitWidget();
				InfoBar = Cast<UAIInfoBar>(InfoBarWidgetComponent->GetUserWidgetObject());
			}
		}
		if (InfoBar)
		{
			InfoBar->ShowLevel(!bHideLevel);
			InfoBar->SetHealthPercent(HealthPercent);
			InfoBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		SetHealthBarColor();
	}
}

void ABuilding::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if(HasAuthority())
	{
		if (Data.NewValue <= 0)
		{
			DestroyBuilding();
		}
	}
	else
	{
		M_ShowInfoBar(true, Data.NewValue / AttributeSet->GetMaxHealth());
	}
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuilding, TeamId);
	DOREPLIFETIME(ABuilding, bHideLevel);
	DOREPLIFETIME(ABuilding, TmpActor);
	DOREPLIFETIME(ABuilding, GridPiece);
}

void ABuilding::Tick(float DeltaSeconds)
{
	if(!HasAuthority())
	{
		ETeamId TempTeamId = UUtil::GetLocalPlayerTeamId(this);
		if(ATeamGridManager* TeamGridManager = ActorManager->GetTeamGridManager(TempTeamId))
		{
			if(TeamGridManager->HasTempActor(GridPiece))
			{
				TeamGridManager->HideTempActor(GridPiece, bShouldShow);
			}
		}
	}
	Super::Tick(DeltaSeconds);
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (GetWorld())
		{
			if(AWorldItem* WorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(DropItemClass, GetActorTransform()))
			{
				WorldItem->InitItem(DropItemName, 10);
				WorldItem->FinishSpawning(GetActorTransform());
			}
			if (DestroyParticles)
			{
				GetWorld()->SpawnActor(DestroyParticles, &GetActorTransform());
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABuilding::SetHealthBarColor()
{
	if (AController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (ARTPlayerState* PS = PC->GetPlayerState<ARTPlayerState>())
		{
			if (TeamId == PS->GetTeamId())
			{
				InfoBar->SetColor(FColor::Green);
			}
			else
			{
				InfoBar->SetColor(FColor::Red);
			}
		}
	}
}

UAbilitySystemComponent* ABuilding::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

URTBaseAttributeSet* ABuilding::GetAttributeSet() const
{
	return AttributeSet;
}
