// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "RTHeroAttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class RADIANT_API URTHeroAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	// Current Health, when 0 we expect owner to die unless prevented by an ability. Capped by MaxHealth.
	// Positive changes can directly use this.
	// Negative changes to Health should go through Damage meta attribute.
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, Health)

	// MaxHealth is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, Mana)
	
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, MaxMana)
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, MovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = OnRep_MaxMovementSpeed)
	FGameplayAttributeData MaxMovementSpeed;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, MaxMovementSpeed)

	UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, Damage)

	UPROPERTY(BlueprintReadOnly, Category = "Repsawn", ReplicatedUsing = OnRep_CurrentRespawnTime)
	FGameplayAttributeData CurrentRespawnTime;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, CurrentRespawnTime)

	UPROPERTY(BlueprintReadOnly, Category = "Repsawn", ReplicatedUsing = OnRep_MaxRespawnTime)
	FGameplayAttributeData MaxRespawnTime;
	ATTRIBUTE_ACCESSORS(URTHeroAttributeSetBase, MaxRespawnTime)
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	
protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);

	UFUNCTION()
	virtual void OnRep_MaxMovementSpeed(const FGameplayAttributeData& OldMaxMovementSpeed);
	
	UFUNCTION()
	virtual void OnRep_AttackDamage(const FGameplayAttributeData& OldDamage);

	UFUNCTION()
	virtual void OnRep_CurrentRespawnTime(const FGameplayAttributeData& OldCurrentRespawnTime);

	UFUNCTION()
	virtual void OnRep_MaxRespawnTime(const FGameplayAttributeData& OldMaxRespawnTime);
	
	UFUNCTION()
	void UpdateMovementSpeed();
};
