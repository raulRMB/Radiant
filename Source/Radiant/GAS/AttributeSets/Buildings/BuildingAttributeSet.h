// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BuildingAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class RADIANT_API UBuildingAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	UPROPERTY(ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	
public:
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UBuildingAttributeSet, AttackDamage)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
private:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);
};
