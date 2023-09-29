// Copyright Radiant Studios


#include "AttributeChangeDisplay.h"

#include "Components/WidgetComponent.h"
#include "UI/InGame/AttributeDisplay/AttributeDisplay.h"
#include "Util/Util.h"


// Sets default values
AAttributeChangeDisplay::AAttributeChangeDisplay()
{
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(FName("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
}

bool AAttributeChangeDisplay::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	if(UAttributeDisplay* Display = Cast<UAttributeDisplay>(WidgetComponent->GetUserWidgetObject()))
	{
		FText DisplayText = FText::FromString(FString::FromInt(FMath::Abs(static_cast<int>(Parameters.RawMagnitude))));
		FLinearColor DisplayColor = Parameters.RawMagnitude > 0 ? FLinearColor::Green : FLinearColor::Red;
		Display->Init(DisplayText, DisplayColor, this);
	}
	
	return Super::OnActive_Implementation(MyTarget, Parameters);
}


void AAttributeChangeDisplay::OnAnimFinished(UUMGSequencePlayer& Player)
{
	RTPRINT("FINISHED");
	Destroy();
}