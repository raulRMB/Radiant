// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldItem.generated.h"

UCLASS()
class RADIANT_API AWorldItem : public AActor
{
	GENERATED_BODY()	

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class USphereComponent* PickUpRadius;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UNiagaraComponent* FX;

	UPROPERTY(VisibleAnywhere, Category = Components, meta = (AllowPrivateAccess =true))
	class UWidgetComponent* NameWidget;

	UPROPERTY(EditAnywhere)
	class UDataTable* ItemDataTable;
protected:
	FString ItemName;
public:
	void InitItem(FString Name);
	
	void PickUp(class ICarrier* Carrier);
public:	
	AWorldItem();

	FString GetItemName() const { return ItemName; }
protected:
	virtual void BeginPlay() override;
};
