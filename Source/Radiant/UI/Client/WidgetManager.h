// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetManager.generated.h"

DECLARE_MULTICAST_DELEGATE(FWidgetSwitchPage)

UCLASS()
class RADIANT_API AWidgetManager : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Widgets")
	FName DefaultWidget;
	
	UPROPERTY(EditAnywhere, Category = "Widgets")
	TMap<FName, TSubclassOf<UUserWidget>> Widgets;
public:	
	// Sets default values for this actor's properties
	AWidgetManager();
	FWidgetSwitchPage OnWidgetSwitchPage;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SwitchTo(const FString& Name);
	void SwitchTo(const char* Name);
	void SwitchTo(const FName& Name);
};
