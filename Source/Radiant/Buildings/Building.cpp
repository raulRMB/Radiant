#include "Building.h"

#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/AbilitySystemComponent/RTAbilitySystemComponent.h"
#include "GAS/AttributeSets/BuildingAttributeSet.h"
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
	AttributeSet->InitMaxHealth(500.f);
	AttributeSet->InitHealth(500.f);
	AttributeSet->InitAttackDamage(10.f);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if(!HasAuthority())
	{
		if(ensureMsgf(InfoBarWidgetComponent, TEXT("No info widget component")))
		{
			InfoBar = Cast<UAIInfoBar>(InfoBarWidgetComponent->GetUserWidgetObject());
			if(InfoBar)
			{
				InfoBar->SetHealthPercent(1.f);
			}
		}
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ABuilding::OnHealthChanged);
}


void ABuilding::GiveInitialAbilities()
{
	for(auto Ability : Abilities)
	{
		AbilitySystemComponent->GiveAbility(Ability);
	}
}

void ABuilding::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if(!HasAuthority() && AttributeSet)
	{
		InfoBar->SetHealthPercent(Data.NewValue / AttributeSet->GetMaxHealth());
	}
}

void ABuilding::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UAbilitySystemComponent* ABuilding::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBuildingAttributeSet* ABuilding::GetAttributeSet() const
{
	return AttributeSet;
}


