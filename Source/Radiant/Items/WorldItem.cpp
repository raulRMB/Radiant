// Copyright Radiant Studios


#include "Items/WorldItem.h"

#include "NiagaraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "ItemInfo/WorldItemInfoWidget.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"
#include "Util/Interfaces/Carrier.h"

AWorldItem::AWorldItem()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	
	PickUpRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpRadius"));
	PickUpRadius->SetupAttachment(Mesh);

	FX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FX"));
	FX->SetupAttachment(Mesh);

	NameWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("NameWidget"));
	NameWidget->SetupAttachment(Mesh);
}

void AWorldItem::InitItem(FName NewItemName, uint32 NewAmount)
{
	ItemName = NewItemName;
	Amount = NewAmount;
}

void AWorldItem::PickUp(ICarrier* Carrier)
{
	TArray<AActor*> OverlappingActors;
	PickUpRadius->GetOverlappingActors(OverlappingActors, AActor::StaticClass());
	if(AActor* CarrierActor = Cast<AActor>(Carrier))
	{
		if(OverlappingActors.Contains(CarrierActor))
		{
			Carrier->PickUpItem(ItemName, Amount);
			Destroy();
		}
	}
}

void AWorldItem::BeginPlay()
{
	Super::BeginPlay();

	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetUserWidgetObject()))
	{
		FString ItemNameString = ItemName.ToString();
		ItemNameString.Append(" : ");
		ItemNameString.Append(FString::FromInt(Amount));
		ItemInfoWidget->SetText(FName(*ItemNameString));
	}

	if(!HasAuthority() && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(SetBackroundTimerHandle, this, &AWorldItem::SetBackgroundSize, 0.01f, false);
	}
}


void AWorldItem::SetBackgroundSize()
{
	if(UWorldItemInfoWidget* ItemInfoWidget = Cast<UWorldItemInfoWidget>(NameWidget->GetUserWidgetObject()))
	{
		ItemInfoWidget->SetBackgroundSize();
	}
}

void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldItem, ItemName);
	DOREPLIFETIME(AWorldItem, Amount);
}


