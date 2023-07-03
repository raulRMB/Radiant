// Copyright Radiant Studios

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GridManager.generated.h"

UENUM()
enum EEnvironmentType
{
	EEnvironmentType_Empty = 0,
	EEnvironmentType_Wall,
	EEnvironmentType_Tower,
	EEnvironmentType_Tree,
	EEnvironmentType_Rock
};

USTRUCT()
struct FGridPiece
{
	GENERATED_BODY()

	EEnvironmentType Type;
	uint32 Level;
	FIntVector2 Position;
	uint8 Size;
};

/**
 * 
 */
UCLASS()
class RADIANT_API UGridManager : public UObject
{
	GENERATED_BODY()

	FIntVector2 GridSize;
	UPROPERTY()
	TArray<class ABuilding*> Pieces;
	TArray<bool> Cells;

	UPROPERTY(EditAnywhere)
	uint8 bShouldCreate : 1;

	UPROPERTY(EditAnywhere)
	TMap<TEnumAsByte<EEnvironmentType>, TSubclassOf<class ABuilding>> BuildingTypes;
public:	
	void InitGrid(int Width, int Height);

	void DrawGrid();
	
	uint32 CellSize = 200;
	uint32 CellHalfSize = CellSize / 2;

	void PlacePieceAtMouse(FGridPiece Piece);
};
