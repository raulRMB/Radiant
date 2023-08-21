// Copyright Radiant Studios


#include "Combat/Artillery/AreaOfEffect.h"
#include "Player/Avatar.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAreaOfEffect::AAreaOfEffect()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	HitBox = CreateDefaultSubobject<USphereComponent>(FName("Mesh"));
	SetRootComponent(HitBox);

	TimerDisplay = CreateDefaultSubobject<UStaticMeshComponent>(FName("TimerDisplay"));
	TimerDisplay->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TimerDisplay->SetupAttachment(HitBox);
}

void AAreaOfEffect::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AAreaOfEffect::ApplyGameplayEffectsToTargets()
{
	if (HasAuthority())
	{
		for (ITeamMember* TeamMember : EffectTargets)
		{
			if (IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(TeamMember))
			{
				if (UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
				{
					ApplyGameplayEffectsToASC(AbilitySystemComponent);
				}
			}
		}
	}
}

void AAreaOfEffect::ApplyGameplayEffectsToASC(UAbilitySystemComponent* ASC)
{
	if(!HasAuthority())
	{
		return;
	}
	
	for (TSubclassOf<UGameplayEffect>& GameplayEffect : GameplayEffects)
	{
		UAbilitySystemComponent* SourceASC = SourceCharacter->GetAbilitySystemComponent();
		SourceASC->ApplyGameplayEffectToTarget(
			GameplayEffect.GetDefaultObject(), ASC, 1, ASC->MakeEffectContext());
	}
}

void AAreaOfEffect::RemoveGameplayEffectsFromASC(UAbilitySystemComponent* ASC)
{
	if(!HasAuthority())
	{
		return;
	}
	
	FGameplayTagContainer TagsToRemove;
	for (TSubclassOf<UGameplayEffect>& GameplayEffect : GameplayEffects)
	{
		for(FGameplayTag Tag : GameplayEffect.GetDefaultObject()->InheritableGameplayEffectTags.CombinedTags)
		{
			TagsToRemove.AddTag(Tag);
		}	
	}
	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(TagsToRemove);
	ASC->RemoveActiveEffects(Query, 1);
}

void AAreaOfEffect::OnHitBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ApplicationType == EAOEApplicationType::Constant || ApplicationType == EAOEApplicationType::OnOverlap)
	{
		if(ShouldHit(OtherActor))
		{
			if(IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
			{
				if(UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
				{
					ApplyGameplayEffectsToASC(AbilitySystemComponent);
				}
			}
		}
	}
}

void AAreaOfEffect::OnHitBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ApplicationType == EAOEApplicationType::Constant)
	{
		if(ShouldHit(OtherActor))
		{
			if(IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
			{
				if(UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
				{
					RemoveGameplayEffectsFromASC(AbilitySystemComponent);
				}
			}
		}
	}
}

void AAreaOfEffect::ApplyInstantEffects()
{
	TArray<UPrimitiveComponent*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	UKismetSystemLibrary::SphereOverlapComponents(this, HitBox->GetComponentLocation(), HitBox->GetScaledSphereRadius(),
	                                              ObjectTypes, UCapsuleComponent::StaticClass(), TArray<AActor*>(),
	                                              OverlappingActors);
	for (auto Component : OverlappingActors)
	{
		auto Actor = Component->GetAttachmentRootActor();
		if (ShouldHit(Actor))
		{
			if (ITeamMember* TeamMember = Cast<ITeamMember>(Actor))
			{
				EffectTargets.AddUnique(TeamMember);
			}
		}
	}

	if (SourceCharacter)
	{
		FGameplayCueParameters CueParameters;
		CueParameters.Location = GetActorLocation();
		SourceCharacter->GetAbilitySystemComponent()->ExecuteGameplayCue(CueTag, CueParameters);
	}

	ApplyGameplayEffectsToTargets();
	Destroy();
}

void AAreaOfEffect::OnTimerEnd()
{
	if (ApplicationType == EAOEApplicationType::Constant)
	{
		Destroy();
	}
	else if(ApplicationType == EAOEApplicationType::OnTimerEnd_Instant)
	{
		ApplyInstantEffects();
	}
	else if(ApplicationType == EAOEApplicationType::OnTimerEnd_Constant)
	{
		ApplicationType = EAOEApplicationType::Constant;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAreaOfEffect::OnTimerEnd, ConstantLifeSpan, false);
	}
}


void AAreaOfEffect::BeginPlay()
{
	Super::BeginPlay();

	CashedTimerScale = TimerDisplay->GetRelativeScale3D();
	
	if (TimerDisplay)
	{
		TimerDisplay->SetRelativeScale3D(FVector::ZeroVector);
	}

	if (ApplicationType == EAOEApplicationType::Instant)
	{
		ApplyInstantEffects();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AAreaOfEffect::OnTimerEnd, LifeSpan, false);
	}

	if(IsValid(this))
	{
		HitBox->OnComponentBeginOverlap.AddDynamic(this, &AAreaOfEffect::OnHitBoxBeginOverlap);
		HitBox->OnComponentEndOverlap.AddDynamic(this, &AAreaOfEffect::OnHitBoxEndOverlap);
	}
}

// Called every frame
void AAreaOfEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TimerDisplay)
	{
		TimerDisplay->SetRelativeScale3D(
			CashedTimerScale * (GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle) / LifeSpan));
	}
}
