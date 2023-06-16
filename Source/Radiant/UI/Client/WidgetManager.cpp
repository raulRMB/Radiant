// Copyright Radiant Studios


#include "WidgetManager.h"

#include "UI/Client/ClientSubsystem.h"
#include "Components/SlateWrapperTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

// Sets default values
AWidgetManager::AWidgetManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWidgetManager::BeginPlay()
{
	Super::BeginPlay();

	for(auto Widget : Widgets)
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), Widget.Value, Widget.Key);
		WidgetInstance->AddToViewport();
		WidgetInstance->SetVisibility(ESlateVisibility::Hidden);
	}

	if(GetGameInstance()->GetSubsystem<UClientSubsystem>()->GetIsLoggedIn())
		SwitchTo("LobbyMenu");
	else
		SwitchTo(DefaultWidget);
}

// Called every frame
void AWidgetManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWidgetManager::SwitchTo(const FString& Name)
{
	OnWidgetSwitchPage.Broadcast();
	TArray<UUserWidget*> AllWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllWidgets, UUserWidget::StaticClass(), false);

	for(auto Widget : AllWidgets)
	{
		if(Widget->GetName() == Name)
		{
			Widget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Widget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AWidgetManager::SwitchTo(const char* Name)
{
	SwitchTo(FString(Name));
}

void AWidgetManager::SwitchTo(const FName& Name)
{
	SwitchTo(Name.ToString());
}

