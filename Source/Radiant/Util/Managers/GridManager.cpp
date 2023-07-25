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

void AGridManager::InitGrid()
{
	Cells.Init(false, GridSize.X * GridSize.Y);

	if(HasAuthority())
	{
		for(int y = 0; y < GridSize.Y; y++)
		{
			for(int x = 0; x < GridSize.X; x++)
			{
				int32 Rand = FMath::RandRange(0, 400);
				EEnvironmentType Type = EEnvironmentType::EEnvironmentType_Empty;
				if(Rand < 2)
				{
					Type = EEnvironmentType::EEnvironmentType_Tree;
				}
				else if(Rand < 4)
				{
					Type = EEnvironmentType::EEnvironmentType_TreeStump;
				}
				else if(Rand < 6)
				{
					Type = EEnvironmentType::EEnvironmentType_Grass;
				}
				else if(Rand < 8)
				{
					Type = EEnvironmentType::EEnvironmentType_Rock;
				}
				else if(Rand < 10)
				{
					Type = EEnvironmentType::EEnvironmentType_Pebbles;
				}
				else
				{
					continue;
				}
				FTransform Transform = FTransform(FVector(x, y, 0.f) * CellSize);
				if(BuildingTypes.Contains(Type))
				{
					GetWorld()->SpawnActor<ABuilding>(BuildingTypes[Type], Transform);
				}
			}
		}
	}
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

bool AGridManager::CheckCanPlace(const FIntVector2 Position)
{
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridSize.X || Position.Y >= GridSize.Y)
	{
		return false;
	}
	int32 Pos = Position.X + Position.Y * GridSize.Y;
	if(Cells.Num() < Pos || Cells[Pos])
	{
		return false;
	}
	
	return Cells[Position.X + Position.Y * GridSize.Y];
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGridManager, Cells);
}
