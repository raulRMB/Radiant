// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "RTBaseAttributeSet.h"
#include "RTAvatarAttributeSet.generated.h"



UCLASS()
class RADIANT_API URTAvatarAttributeSet : public URTBaseAttributeSet
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(URTAvatarAttributeSet, Mana)
	
	UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(URTAvatarAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "Repsawn", ReplicatedUsing = OnRep_CurrentRespawnTime)
	FGameplayAttributeData CurrentRespawnTime;
	ATTRIBUTE_ACCESSORS(URTAvatarAttributeSet, CurrentRespawnTime)

	UPROPERTY(BlueprintReadOnly, Category = "Repsawn", ReplicatedUsing = OnRep_MaxRespawnTime)
	FGameplayAttributeData MaxRespawnTime;
	ATTRIBUTE_ACCESSORS(URTAvatarAttributeSet, MaxRespawnTime)	

	UPROPERTY(BlueprintReadOnly, Category = "Currency", ReplicatedUsing = OnRep_Radianite)
	FGameplayAttributeData Radianite;
	ATTRIBUTE_ACCESSORS(URTAvatarAttributeSet, Radianite)
	
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	virtual void OnRep_CurrentRespawnTime(const FGameplayAttributeData& OldCurrentRespawnTime);

	UFUNCTION()
	virtual void OnRep_MaxRespawnTime(const FGameplayAttributeData& OldMaxRespawnTime);

	UFUNCTION()
	virtual void OnRep_Radianite(const FGameplayAttributeData& OldRadianite);
};
