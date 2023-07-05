// Copyright Radiant Studios


#include "GAS/Abilities/RTAbility.h"

#include "Player/Avatar.h"
#include "Util/Util.h"

void URTAbility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

FVector URTAbility::GetRangeBasedMouseLocation()
{
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector Direction = GetAvatarToMouseDirection();
	
	return Direction.GetClampedToSize(0.0, MaxRange) + AvatarLocation * FVector(1, 1, 0);
}

FVector URTAbility::GetRangedBaseMouseLocationWithHeroHalfHeight()
{
	if(AAvatar* Hero = Cast<AAvatar>(GetAvatarActorFromActorInfo()))
	{
		return GetRangeBasedMouseLocation() + Hero->GetHalfHeightVector();
	}
	RTLOG("Avatar is not a hero")
	RTPRINT("Avatar is not a hero")
	return FVector::ZeroVector;
}

FVector URTAbility::GetAvatarToMouseDirection()
{
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	AvatarLocation.Z = 0;
	FVector MouseLocation = GetMouseWorldLocation();
	MouseLocation.Z = 0;
	return MouseLocation - AvatarLocation;
}

bool URTAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bBlocked = false;
	bool bMissing = false;
	static FGameplayTagContainer AbilitySystemComponentTags;
	AbilitySystemComponentTags.Reset();

	AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);
	if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
	{
		bBlocked = true;
	}
	
	if (ActivationBlockedTags.Num() || ActivationRequiredTags.Num())
	{
		if (AbilitySystemComponentTags.HasAny(ActivationBlockedTags))
		{
			GetMatchingTags(AbilitySystemComponentTags, ActivationBlockedTags, OptionalRelevantTags);
			bBlocked = true;
		}

		if (!AbilitySystemComponentTags.HasAll(ActivationRequiredTags))
		{
			GetMissingTags(AbilitySystemComponentTags, ActivationRequiredTags, OptionalRelevantTags);
			bMissing = true;
		}
	}

	if (SourceTags != nullptr)
	{
		if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
		{
			if (SourceTags->HasAny(SourceBlockedTags))
			{
				GetMatchingTags(*SourceTags, SourceBlockedTags, OptionalRelevantTags);
				bBlocked = true;
			}

			if (!SourceTags->HasAll(SourceRequiredTags))
			{
				GetMissingTags(*SourceTags, SourceRequiredTags, OptionalRelevantTags);
				bMissing = true;
			}
		}
	}

	if (TargetTags != nullptr)
	{
		if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
		{
			if (TargetTags->HasAny(TargetBlockedTags))
			{
				GetMatchingTags(*TargetTags, TargetBlockedTags, OptionalRelevantTags);
				bBlocked = true;
			}

			if (!TargetTags->HasAll(TargetRequiredTags))
			{
				GetMissingTags(*TargetTags, TargetRequiredTags, OptionalRelevantTags);
				bMissing = true;
			}
		}
	}

	return !(bBlocked || bMissing);
}

void URTAbility::GetMatchingTags(const FGameplayTagContainer& Tags1, const FGameplayTagContainer& Tags2, FGameplayTagContainer* OptionalRelevantTags)
{
	if (Tags1.IsEmpty() || Tags2.IsEmpty())
	{
		return;
	}
	TArray<FGameplayTag> Tags;
	Tags1.GetGameplayTagArray(Tags);
	for (const FGameplayTag& Tag : Tags)
	{
		if (Tags2.HasTag(Tag))
		{
			OptionalRelevantTags->AddTag(Tag);
		}
	}
}

void URTAbility::GetMissingTags(const FGameplayTagContainer& Tags1, const FGameplayTagContainer& Tags2, FGameplayTagContainer* OptionalRelevantTags)
{
	if (Tags1.IsEmpty() || Tags2.IsEmpty())
	{
		return;
	}
	TArray<FGameplayTag> Tags;
	Tags1.GetGameplayTagArray(Tags);
	for (const FGameplayTag& Tag : Tags)
	{
		if (!Tags2.HasTag(Tag))
		{
			OptionalRelevantTags->AddTag(Tag);
		}
	}
}

FGameplayTag URTAbility::GetTriggerTag() const
{
	return AbilityTriggers[0].TriggerTag;
}

FGameplayTag URTAbility::GetCooldownTag() const
{
	if(GetCooldownGameplayEffect())
	{
		return GetCooldownGameplayEffect()->InheritableOwnedTagsContainer.Added.First();
	}
	return FGameplayTag();
}
