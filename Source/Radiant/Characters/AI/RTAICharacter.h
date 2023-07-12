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

	UPROPERTY()
	class UAIInfoBar* OverHeadInfoBar;
public:
	ARTAICharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	class UWidgetComponent* OverHeadInfoBarWidgetComponent;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UNPCAttributeSet* GetAttributeSet() const;
	
	virtual void SetIsDead(const bool NewIsDead) override;

	UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
protected:
	void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	
	virtual void BeginPlay() override;

};
