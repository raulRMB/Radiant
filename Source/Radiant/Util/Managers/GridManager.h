// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamId.h"
#include "Enums/EnvironmentType.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

USTRUCT()
struct FGridPiece
{
	GENERATED_BODY()

	UPROPERTY()
	EEnvironmentType Type;
	UPROPERTY()
	uint32 Level;
	UPROPERTY()
	FIntVector2 Position;
	UPROPERTY()
	uint8 Size;
	UPROPERTY()
	ETeamId TeamId;

	FGridPiece() :
		Type(EEnvironmentType::EEnvironmentType_Empty),
		Level(0),
		Position(FIntVector2(0, 0)),
		Size(1),
		TeamId(ETeamId::Neutral)
	{}
};


UCLASS()
class RADIANT_API AGridManager : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class AActor*> Pieces;
	UPROPERTY(VisibleAnywhere, Replicated)
	TArray<EEnvironmentType> Cells;
	UPROPERTY(VisibleAnywhere)
	uint16 GridDimensions;
	
	UPROPERTY(EditAnywhere)
	uint8 bSpawnMap : 1;

	UPROPERTY(EditAnywhere)
	TMap<EEnvironmentType, TSubclassOf<class AActor>> BuildingTypes;

	float PerlinScale = 0.1f;

	UPROPERTY(EditAnywhere)
	class UTexture2D* MapTexture;
	
public:
	AGridManager();

	UFUNCTION(CallInEditor)
	void GenerateMap();
	void InitGrid();
	
	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	void PlacePieceAtMouse(FGridPiece Piece);
	bool CheckCanPlace(const FGridPiece Piece);

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
