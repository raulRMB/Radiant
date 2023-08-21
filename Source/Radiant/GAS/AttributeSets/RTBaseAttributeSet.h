// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RTBaseAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


UCLASS()
class RADIANT_API URTBaseAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Defense", ReplicatedUsing = OnRep_MagicResistance)
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MagicResistance)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, AttackDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AbilityPower)
	FGameplayAttributeData AbilityPower;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, AbilityPower)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_CriticalChance)
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, CriticalChance)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_CriticalDamage)
	FGameplayAttributeData CriticalDamage;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, CriticalDamage)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_ArmorPenetration)
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, ArmorPenetration)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_MagicPenetration)
	FGameplayAttributeData MagicPenetration;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MagicPenetration)
	
	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackSpeed)
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, AttackSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = OnRep_AttackRange)
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, AttackRange)
	
	UPROPERTY(BlueprintReadOnly, Category = "XP", ReplicatedUsing = OnRep_XP)
	FGameplayAttributeData XP;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, XP)

	UPROPERTY(BlueprintReadOnly, Category = "XP", ReplicatedUsing = OnRep_MaxXP)
	FGameplayAttributeData MaxXP;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MaxXP)

	UPROPERTY(BlueprintReadOnly, Category = "Utility", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Utility", ReplicatedUsing = OnRep_MaxMovementSpeed)
	FGameplayAttributeData MaxMovementSpeed;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MaxMovementSpeed)
	
	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, Level)

	UPROPERTY(BlueprintReadOnly, Category = "Level", ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, MaxLevel)

	UPROPERTY(BlueprintReadOnly, Category = "Utility", ReplicatedUsing = OnRep_CooldownReduction)
	FGameplayAttributeData CooldownReduction;
	ATTRIBUTE_ACCESSORS(URTBaseAttributeSet, CooldownReduction)

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance);
	
	UFUNCTION()
	virtual void OnRep_AttackDamage(const FGameplayAttributeData& OldDamage);

	UFUNCTION()
	virtual void OnRep_AbilityPower(const FGameplayAttributeData& OldAbilityPower);

	UFUNCTION()
	virtual void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance);

	UFUNCTION()
	virtual void OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage);
	
	UFUNCTION()
	virtual void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration);

	UFUNCTION()
	virtual void OnRep_MagicPenetration(const FGameplayAttributeData& OldMagicPenetration);
	
	UFUNCTION()
	virtual void OnRep_XP(const FGameplayAttributeData& OldXP);

	UFUNCTION()
	virtual void OnRep_MaxXP(const FGameplayAttributeData& OldMaxXP);

	UFUNCTION()
	virtual void OnRep_Level(const FGameplayAttributeData& OldLevel);

	UFUNCTION()
	virtual void OnRep_MaxLevel(const FGameplayAttributeData& OldMaxLevel);

	UFUNCTION()
	virtual void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed);

	UFUNCTION()
	virtual void OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange);
	
	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

	UFUNCTION()
	virtual void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed);

	UFUNCTION()
	virtual void OnRep_CooldownReduction(const FGameplayAttributeData& OldCooldownReduction);
	
	UFUNCTION()
	void UpdateMovementSpeed();
};
