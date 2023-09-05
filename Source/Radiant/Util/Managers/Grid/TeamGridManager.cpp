// Copyright Radiant Studios


#include "TeamGridManager.h"

#include "Net/UnrealNetwork.h"
#include "Structs/GridPiece.h"
#include "Util/TempGridActor.h"
#include "Enums/EnvironmentType.h"
#include "Enums/VIsionRange.h"
#include "Player/RTPlayerController.h"
#include "Util/Util.h"
#include "Util/Interfaces/TeamMember.h"


// Sets default values
ATeamGridManager::ATeamGridManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ATeamGridManager::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
}

void ATeamGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATeamGridManager, Cells);
	DOREPLIFETIME(ATeamGridManager, TeamId);
	DOREPLIFETIME(ATeamGridManager, VisibleActors);
	DOREPLIFETIME(ATeamGridManager, GridManager);
}

bool ATeamGridManager::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	if(const ARTPlayerController* TeamMember = Cast<ARTPlayerController>(RealViewer))
	{
		return TeamMember->GetTeamId() == TeamId;
	}
	return false;
}

// Called every frame
void ATeamGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HasAuthority())
	{
		ClearAllVisible();
		DrawVisible();
	}
	else
	{
		if(TeamId == ETeamId::Neutral || VisibleCells.Num() == 0 || TeamId != UUtil::GetLocalPlayerTeamId(this))
		{
			return;
		}
		ClearAllVisible();
		DrawVisible();
		FTexture2DMipMap* MipMap = &RenderTarget->GetPlatformData()->Mips[0];
		FByteBulkData* RawImageData = &MipMap->BulkData;
		FColor* FormattedImageData = static_cast<FColor*>(RawImageData->Lock(LOCK_READ_WRITE));
		for(int i = 0; i < 128; i++)
		{
			for(int j = 0; j < 128; j++)
			{
				FormattedImageData[i + j * 128] = VisibleCells[i + j * 128] ? FColor::White : FColor::Black;
			}
		}
		RawImageData->Unlock();
		RenderTarget->UpdateResource();	
	}
}

void ATeamGridManager::InitGrid()
{
	VisibleCells.Init(false, 128*128);
	TempPieces.Init(nullptr, 128*128);
}

void ATeamGridManager::AddVisibleActor(AActor* Actor)
{
	VisibleActors.AddUnique(Actor);
}

void ATeamGridManager::RemoveVisibleActor(AActor* Actor)
{
	VisibleActors.Remove(Actor);
}

void ATeamGridManager::SpawnTempActor(FGridPiece& Piece, UStaticMeshComponent* Mesh, FTransform Transform)
{
	if(TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions])
	{
		return;
	}
	ATempGridActor* TempGridActor = GetWorld()->SpawnActorDeferred<ATempGridActor>(ATempGridActor::StaticClass(), Transform);
	TempGridActor->SetMesh(Mesh);
	TempGridActor->SetActorTransform(Mesh->GetComponentTransform());
	TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] = TempGridActor;
	TempGridActor->FinishSpawning(Transform);
}

void ATeamGridManager::DestroyTempActor(FGridPiece& Piece)
{
	if(!TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions])
	{
		return;
	}
	TempPieces[Piece.Position.X + Piece.Position.Y * GridDimensions]->Destroy();
}

void ATeamGridManager::CopyCells(const TArray<EEnvironmentType>& TrueCells)
{
	Cells = TrueCells;
}

bool ATeamGridManager::GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell)
{
	FIntVector2 NeighborPosition = FIntVector2(Position.X + Direction.X, Position.Y + Direction.Y);
	if(NeighborPosition.X < 0 || NeighborPosition.Y < 0 || NeighborPosition.X >= GridDimensions || NeighborPosition.Y >= GridDimensions)
	{
		return false;
	}
	OutCell = Cells[NeighborPosition.X + NeighborPosition.Y * GridDimensions];
	return true;
}

bool ATeamGridManager::IsBlockingVision(const FIntVector2& Position, const FIntVector2& Direction)
{	
	EEnvironmentType& Cell = GetCell(Position);
	if(Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	// TOP RIGHT
	if(Direction == FIntVector2(1, -1) && GetNeighbor(Position, FIntVector2(1, 0), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty
		&& GetNeighbor(Position, FIntVector2(0, -1), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	// TOP LEFT
	if(Direction == FIntVector2(-1, -1) && GetNeighbor(Position, FIntVector2(-1, 0), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty
		&& GetNeighbor(Position, FIntVector2(0, -1), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	// BOTTOM RIGHT
	if(Direction == FIntVector2(1, 1) && GetNeighbor(Position, FIntVector2(1, 0), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty
		&& GetNeighbor(Position, FIntVector2(0, 1), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	// BOTTOM LEFT
	if(Direction == FIntVector2(-1, 1) && GetNeighbor(Position, FIntVector2(-1, 0), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty
		&& GetNeighbor(Position, FIntVector2(0, 1), Cell) && Cell != EEnvironmentType::EEnvironmentType_Empty)
	{
		return true;
	}
	return false;
}

EEnvironmentType& ATeamGridManager::GetCell(const FIntVector2& Position)
{
	return Cells[Position.X + Position.Y * GridDimensions];
}

bool ATeamGridManager::CheckVisible(const FVector2D& From, const FVector2D& To)
{
	const double DX = To.X - From.X;
	const double DY = To.Y - From.Y;

	const double SideLength = FMath::Max(FMath::Abs(DX), FMath::Abs(DY));

	const double XIncrement = DX / SideLength;
	const double YIncrement = DY / SideLength;

	double CurrentX = From.X;
	double CurrentY = From.Y;

	FVector2D Direction = FVector2D(DX, DY);
	Direction.Normalize();

	FIntVector2 DirectionInt = FIntVector2(DX, DY);
	// if (Angle > 0.52359877559 && Angle < 1.0471975512)
	// {
	// 	Direction = FVector2D(1, -1);
	// }
	// else if(Angle > 2.0943951024 && Angle < 2.61799387799)
	// {
	// 	Direction = FVector2D(-1, -1);
	// }
	// else if(Angle > 3.66519142919 && Angle < 4.18879020479)
	// {
	// 	Direction = FVector2D(-1, 1);
	// }
	// else if(Angle > 5.23598775598 && Angle < 5.75958653158)
	// {
	// 	Direction = FVector2D(1, 1);
	// }
	for(int i = 0; i <= SideLength; i++)
	{
		uint32 index = static_cast<uint32>(CurrentX) + static_cast<uint32>(CurrentY) * GridDimensions;
		VisibleCells[index] = true;
		if(IsBlockingVision(FIntVector2(CurrentX, CurrentY), DirectionInt))
		{
			break;
		}
		CurrentX += XIncrement;
		CurrentY += YIncrement;
	}
	return false;
}

bool ATeamGridManager::GetVisibleCell(const FIntVector2& Position)
{
	return VisibleCells[Position.X + Position.Y * GridDimensions];
}

void ATeamGridManager::ClearAllVisible()
{	
	for(int i = 0; i < VisibleCells.Num(); i++)
	{
		if(VisibleCells[i])
		{
			VisibleCells[i] = false;
		}
	}
}

void ATeamGridManager::DrawVisible()
{
	for(auto Actor : VisibleActors)
	{
		if(Actor)
		{
			if(ITeamMember * TeamMember = Cast<ITeamMember>(Actor))
			{
				if(TeamMember->GetTeamId() != TeamId)
				{
					continue;
				}
				if(!IsValid(GridManager))
				{
					return;
				}
				double X = (Actor->GetActorLocation().Y + 100.) / 200.;
				double Y = 127. - (Actor->GetActorLocation().X - 100.) / 200.;
				for(FIntVector2& Vec : GridManager->GetRange(EVisionRange::Long))
				{
					FVector2D From = FVector2D(X, Y);
					FVector2D To = FVector2D(X + Vec.X, Y + Vec.Y);
					CheckVisible(From, To);
				}
			}
		}
	}
}

bool ATeamGridManager::IsTargetVisible(const AActor* Target)
{
	FIntVector2 TargetPosition = FIntVector2((Target->GetActorLocation().Y + 100) / 200, 127 - (Target->GetActorLocation().X + 100) / 200);
	return VisibleCells[TargetPosition.X + TargetPosition.Y * GridDimensions];
}


