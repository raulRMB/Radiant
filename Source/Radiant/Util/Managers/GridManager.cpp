// Copyright Radiant Studios


#include "Util/Managers/GridManager.h"
#include "DrawDebugHelpers.h"
#include "Building/Building.h"
#include "Util/Util.h"

void UGridManager::InitGrid(int Width, int Height)
{
	GridSize = FIntVector2(Width, Height);
	Cells.Init(false, GridSize.X * GridSize.Y);
}

void UGridManager::DrawGrid()
{
	if(!GetWorld())
		return;

	RTLOG("ASD");
}

void UGridManager::PlacePieceAtMouse(FGridPiece Piece)
{
	FVector Mouse = UUtil::GetMousePosition(this, {});
	Mouse.X = FMath::RoundToInt(Mouse.X / CellSize) * CellSize;
	Mouse.Y = FMath::RoundToInt(Mouse.Y / CellSize) * CellSize;
	Mouse.Z = 0;
	
	Piece.Position = FIntVector2(Mouse.X, Mouse.Y);

	GetWorld()->SpawnActor<ABuilding>(BuildingTypes[Piece.Type], Mouse, FRotator::ZeroRotator);
}
