#include "Building.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/BuildingAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/Avatar.h"
#include "UI/AIInfoBar.h"

ABuilding::ABuilding()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UBuildingAttributeSet>("AttributeSet");
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);

	InfoBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("InfoBarWidgetComponent");
	InfoBarWidgetComponent->SetupAttachment(RootComponent);
}

void ABuilding::BeginPlay()
{
	Super::BeginPlay();

	if (!AttributeSet)
	{
		AttributeSet = NewObject<UBuildingAttributeSet>(this);
	}

	GiveInitialAbilities();
	AttributeSet->InitMaxHealth(MaxHealth);
	AttributeSet->InitHealth(MaxHealth);
	AttributeSet->InitAttackDamage(10.f);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (!HasAuthority())
	{
		if (ensureMsgf(InfoBarWidgetComponent, TEXT("No info widget component")))
		{
			InfoBar = Cast<UAIInfoBar>(InfoBarWidgetComponent->GetUserWidgetObject());
			if (InfoBar)
			{
				InfoBar->ShowLevel(!bHideLevel);
				InfoBar->SetHealthPercent(1.f);
			}
		}
		SetHealthBarColor();
	}

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
		Destroy();
	}
}

void ABuilding::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (HasAuthority())
	{
		if (Data.NewValue <= 0)
		{
			Destroy();
		}
	}
	else if (AttributeSet)
	{
		InfoBar->SetHealthPercent(Data.NewValue / AttributeSet->GetMaxHealth());
	}
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuilding, TeamId);
}

void ABuilding::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABuilding::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HasAuthority())
	{
		if (DestroyParticles)
		{
			if (GetWorld())
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
