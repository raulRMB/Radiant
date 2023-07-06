// Copyright Radiant Studios


#include "Items/WorldItem.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ItemInfo/WorldItemInfoWidget.h"
#include "Util/Interfaces/Carrier.h"

AWorldItem::AWorldItem()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	PickUpRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpRadius"));
	PickUpRadius->SetupAttachment(Mesh);

	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(Mesh);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(Mesh);
}

void AWorldItem::InitItem(FName Name)
{
	ItemName = Name;
	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetWidget()))
	{
		ItemInfoWidget->SetText(Name);
	}
}

void AWorldItem::PickUp(ICarrier* Carrier)
{
	TArray<AActor*> OverlappingActors;
	PickUpRadius->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if(AActor* CarrierActor = Cast<AActor>(Carrier))
	{
		if(OverlappingActors.Contains(CarrierActor))
		{
			Carrier->PickUpItem(ItemName);
		}
	}
}

void AWorldItem::BeginPlay()
{
	Super::BeginPlay();
}


