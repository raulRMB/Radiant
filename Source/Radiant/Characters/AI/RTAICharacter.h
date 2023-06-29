// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "Characters/RTCharacter.h"
#include "RTAICharacter.generated.h"


UCLASS()
class RADIANT_API ARTAICharacter : public ARTCharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=BehaviorTree, meta=(AllowPrivateAccess=true))
	class UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class UNPCAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class APickUp> PickUpClass;
public:
	// Sets default values for this character's properties
	ARTAICharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UNPCAttributeSet* GetAttributeSet() const;
	
	virtual void Die() override;

	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
