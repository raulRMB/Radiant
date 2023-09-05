#include "Building.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/Buildings/BuildingAttributeSet.h"
#include "Items/WorldItem.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "Player/RTPlayerState.h"
#include "UI/AIInfoBar.h"
#include "Util/Util.h"
#include "Util/Managers/Grid/TeamGridManager.h"

ABuilding::ABuilding()
{
	bReplicates = true;
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UBuildingAttributeSet>("AttributeSet");
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetupAttachment(GetRootComponent());
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = false;
}

UStaticMeshComponent* ABuilding::GetMesh() const
{
	return Mesh;
}

bool ABuilding::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const
{
	if(const ARTPlayerController* PC = Cast<ARTPlayerController>(RealViewer))
	{
		if(auto PS = PC->GetPlayerState<ARTPlayerState>())
		{
			return IsVisibleForTeam(PS->GetTeamId());
		}
	}
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}


bool ABuilding::IsVisibleForTeamLocal(const ETeamId TargetTeamId) const
{
	if(TargetTeamId == GetTeamId())
	{
		return true;
	}
	if(ATeamGridManager* TeamGridManager = Cast<ATeamGridManager>(UGameplayStatics::GetActorOfClass(this, ATeamGridManager::StaticClass())))
	{
		return TeamGridManager->IsTargetVisible(this);
	}
	return false;
}

bool ABuilding::IsVisibleForTeam(const ETeamId TargetTeamId) const
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, ATeamGridManager::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		if (ATeamGridManager* TeamGridManager = Cast<ATeamGridManager>(Actor))
		{
			if(TeamGridManager->GetTeamId() == TargetTeamId)
			{
				return TeamGridManager->IsTargetVisible(this);
			}
		}
	}
	return false;
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	if (!AttributeSet)
	{
		AttributeSet = NewObject<UBuildingAttributeSet>(this);
	}
	NetUpdateFrequency = 10.f;
	NetCullDistanceSquared = 75000000.0f;
	GiveInitialAbilities();
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(MaxHealth);
	AttributeSet->InitAttackDamage(10.f);

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
		GridManager->ClearPiece(GridPiece);
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
		bShouldShow = IsVisibleForTeamLocal(UUtil::GetLocalPlayerTeamId(this));
		SetActorHiddenInGame(!bShouldShow);
		if(ATeamGridManager* TeamGridManager = Cast<ATeamGridManager>(UGameplayStatics::GetActorOfClass(this, ATeamGridManager::StaticClass())))
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

UBuildingAttributeSet* ABuilding::GetAttributeSet() const
{
	return AttributeSet;
}
