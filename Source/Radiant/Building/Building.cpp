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

ABuilding::ABuilding()
{
	bReplicates = true;
	AbilitySystemComponent = CreateDefaultSubobject<URTAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AttributeSet = CreateDefaultSubobject<UBuildingAttributeSet>("AttributeSet");
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>("CapsuleComponent");
	SetRootComponent(CapsuleComponent);
	bAlwaysRelevant = true;
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
		Destroy();
	}
}

void ABuilding::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if(HasAuthority())
	{
		if (Data.NewValue <= 0)
		{
			Destroy();
		}
	}
	else
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
			InfoBar->SetHealthPercent(Data.NewValue / AttributeSet->GetMaxHealth());
			InfoBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		SetHealthBarColor();
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
		if (GetWorld())
		{
			if(AWorldItem* WorldItem = GetWorld()->SpawnActorDeferred<AWorldItem>(DropItemClass, GetActorTransform()))
			{
				WorldItem->InitItem(DropItemName, 5);
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
