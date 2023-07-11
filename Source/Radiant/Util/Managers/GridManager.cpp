// Copyright Radiant Studios


#include "Util/Managers/GridManager.h"
#include "DrawDebugHelpers.h"
#include "Building/Building.h"
#include "Net/UnrealNetwork.h"
#include "Util/Util.h"

AGridManager::AGridManager()
{
	bReplicates = true;
}

void AGridManager::InitGrid(int Width, int Height)
{
	GridSize = FIntVector2(Width, Height);
	Cells.Init(false, GridSize.X * GridSize.Y);
	// if(GetWorld() && HasAuthority())
	// {
	// 	for(int i = 0; i < Cells.Num(); i++)
	// 	{
	// 		double X = static_cast<double>(i % GridSize.X) / static_cast<double>(GridSize.X);
	// 		double Y = static_cast<double>(i / GridSize.Y) / static_cast<double>(GridSize.Y);
	// 		FVector2D Vec = FVector2D(X, Y);
	// 		
	// 		// RTLOGP("%s", *Vec.ToString());
	// 		if(FMath::PerlinNoise2D(Vec) > .425f)
	// 		{
	// 			GetWorld()->SpawnActor<ABuilding>(BuildingTypes[EEnvironmentType::EEnvironmentType_Tree], FTransform(FVector(i % GridSize.X, i / GridSize.Y, 0.f) * CellSize));
	// 		}
	// 	}
	// }
}

void AGridManager::PlacePieceAtMouse(FGridPiece Piece)
{
	if(HasAuthority())
	{
		if(Piece.Position.X < 0 || Piece.Position.Y < 0 || Piece.Position.X >= GridSize.X || Piece.Position.Y >= GridSize.Y)
		{
			return;
		}
		auto Pos = Piece.Position.X + Piece.Position.Y * GridSize.Y;
		if(Cells.Num() < Pos || Cells[Pos])
		{
			return;
		}
		FTransform Transform = FTransform(FVector(Piece.Position.X, Piece.Position.Y, 0.f) * CellSize);
		ABuilding* Building = GetWorld()->SpawnActorDeferred<ABuilding>(BuildingTypes[Piece.Type], Transform);
		Building->S_SetTeamId(Piece.TeamId);
		Building->FinishSpawning(Transform);
		Cells[Piece.Position.X + Piece.Position.Y * GridSize.Y] = true;
	}
}

bool AGridManager::CheckCanPlace(const FGridPiece Piece)
{
	return Cells[Piece.Position.X + Piece.Position.Y * GridSize.Y];
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	InitGrid(128, 128);
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGridManager, Cells);
}
