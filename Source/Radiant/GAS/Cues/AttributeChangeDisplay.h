// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "AttributeChangeDisplay.generated.h"

UCLASS()
class RADIANT_API AAttributeChangeDisplay : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;
	
public:
	AAttributeChangeDisplay();

protected:
	virtual bool OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) override;
private:
	friend class UAttributeDisplay;
	void OnAnimFinished(class UUMGSequencePlayer& Player);
};
