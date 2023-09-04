// Copyright Radiant Studios


#include "Util/Managers/GridManager.h"
#include "DrawDebugHelpers.h"
#include "Building/Building.h"
#include "Engine/Canvas.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Modes/Base/RTGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/RTPlayerState.h"
#include "Util/TempGridActor.h"
#include "Util/Util.h"

TArray<bool>& AGridManager::GetVisibleCellsArray(ETeamId TeamId)
{
	return TeamId == ETeamId::Red ? VisibleCellsRedTeam : VisibleCellsBlueTeam;
}

AGridManager::AGridManager(): GridDimensions(0), bSpawnMap(0), MapTexture(nullptr), RenderTarget(nullptr),
                              EmptyTexture(nullptr),
                              VisionTexture(nullptr)
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::GenerateMap()
{
#if !UE_BUILD_SHIPPING

	Cells.Empty();
	Cells.Init(EEnvironmentType::EEnvironmentType_Empty, MapTexture->GetSizeX() * MapTexture->GetSizeY());
	GridDimensions = MapTexture->GetSizeX();
	
	TextureCompressionSettings OldCompressionSettings = MapTexture->CompressionSettings;
	ETextureSourceEncoding OldSourceEncoding = MapTexture->SourceColorSettings.EncodingOverride;
	TextureMipGenSettings OldMipGenSettings = MapTexture->MipGenSettings;
	bool OldSRGB = MapTexture->SRGB;

	MapTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	MapTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	MapTexture->SourceColorSettings.EncodingOverride = ETextureSourceEncoding::TSE_Linear;
	MapTexture->SRGB = false;
	MapTexture->UpdateResource();
	
	const FColor* FormatedImageData = static_cast<const FColor*>(MapTexture->GetPlatformData()->Mips[0].BulkData.LockReadOnly());
	
	for(int32 X = 0; X < MapTexture->GetSizeX(); X++)
	{
		for (int32 Y = 0; Y < MapTexture->GetSizeY(); Y++)
		{
			FColor PixelColor = FormatedImageData[Y * MapTexture->GetSizeX() + X];

			if(PixelColor.ToPackedRGBA() == 0Xffffffff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Wall;
			}
			else if(PixelColor.ToPackedRGBA() == 0Xff6f00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Rock;
			}
			else if(PixelColor.ToPackedRGBA() == 0X3bff00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Tree;
			}
			else if(PixelColor.ToPackedRGBA() == 0x0000ffff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Vein;
			}
			else if(PixelColor.ToPackedRGBA() == 0xfbff00ff)
			{
				Cells[MapTexture->GetSizeY() * Y + X] = EEnvironmentType::EEnvironmentType_Grass;
			}
		}
	}

	MapTexture->GetPlatformData()->Mips[0].BulkData.Unlock();

	MapTexture->CompressionSettings = OldCompressionSettings;
	MapTexture->MipGenSettings = OldMipGenSettings;
	MapTexture->SourceColorSettings.EncodingOverride = OldSourceEncoding;
	MapTexture->SRGB = OldSRGB;
	MapTexture->UpdateResource();
	
#endif // WITH_EDITOR
	
}

void AGridManager::SpawnTmpActor(FGridPiece& Piece, UStaticMeshComponent* Mesh, FTransform Transform)
{
	if(TmpCells[Piece.Position.X + Piece.Position.Y * GridDimensions] == Piece.Type)
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Spawning TmpActor")));
	FTransform SpawnTransform = FTransform(GetTransformedVector(Piece.Position));
	TmpCells[Piece.Position.X + Piece.Position.Y * GridDimensions] = Piece.Type;
	auto tmp = GetWorld()->SpawnActorDeferred<ATempGridActor>(ATempGridActor::StaticClass(), SpawnTransform);
	tmp->SetMesh(Mesh);
	//tmp->GetMesh()->BodyInstance->Bo
	//tmp->GetMesh()->BodyInstance.Scale3D = Mesh->BodyInstance.Scale3D;
	tmp->SetActorTransform(Transform);
	TmpPieces[Piece.Position.X + Piece.Position.Y * GridDimensions] = tmp;
	tmp->FinishSpawning(SpawnTransform);
}

void AGridManager::DestroyTmpActor(FGridPiece& Piece)
{
	if(TmpCells[Piece.Position.X + Piece.Position.Y * GridDimensions] == EEnvironmentType::EEnvironmentType_Empty)
	{
		return;
	}
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Red, FString::Printf(TEXT("Destorying TmpActor")));
	TmpCells[Piece.Position.X + Piece.Position.Y * GridDimensions] = EEnvironmentType::EEnvironmentType_Empty;
	TmpPieces[Piece.Position.X + Piece.Position.Y * GridDimensions]->Destroy();
}

void AGridManager::InitGrid()
{

	VisibleCellsRedTeam.Init(false, 128*128);
	VisibleCellsBlueTeam.Init(false, 128*128);
	TmpCells.Init(EEnvironmentType::EEnvironmentType_Empty, MapTexture->GetSizeX() * MapTexture->GetSizeY());
	TmpPieces.Init(nullptr, MapTexture->GetSizeX() * MapTexture->GetSizeY());
	if(!bSpawnMap)
	{
		return;
	}
	
	if(HasAuthority())
	{
		for(int x = 0; x < GridDimensions; x++)
		{
			for (int y = 0; y < GridDimensions; y++)
			{
				if(Cells[y * GridDimensions + x] == EEnvironmentType::EEnvironmentType_Empty)
				{
					continue;
				}
				FIntVector2 Position = FIntVector2(x, y);
				FTransform SpawnTransform = FTransform(GetTransformedVector(Position));			
				AActor* Actor = GetWorld()->SpawnActorDeferred<AActor>(BuildingTypes[Cells[y * GridDimensions + x]], SpawnTransform);
				
				if(ABuilding* Building = Cast<ABuilding>(Actor))
				{
					FGridPiece Piece;
					Piece.Position = Position;
					Piece.Level = 1;
					Piece.Type = Cells[y * GridDimensions + x];
					Piece.TeamId = ETeamId::Neutral;
					Building->InitGridValues(this, Piece);
					Building->S_SetTeamId(ETeamId::Neutral);
				}				
				
				Actor->FinishSpawning(SpawnTransform);
			}
		}
	}
}

void AGridManager::AddVisibleActor(AActor* Actor)
{
	VisibleActors.Add(Actor);
}

void AGridManager::S_RequestInitGrid_Implementation(ARTPlayerState* PlayerState)
{
	PlayerState->C_InitGrid(Cells);
}

void AGridManager::PlacePieceAtMouse(const FGridPiece Piece)
{
	if(HasAuthority())
	{
		if(Piece.Position.X < 0 || Piece.Position.Y < 0 || Piece.Position.X >= GridDimensions || Piece.Position.Y >= GridDimensions)
		{
			return;
		}
		int32 Pos = Piece.Position.X + Piece.Position.Y * GridDimensions;
		if(Cells.Num() < Pos)
		{
			return;
		}
		if(Piece.Type == EEnvironmentType::EEnvironmentType_Extractor)
		{
			EEnvironmentType Type = Cells[Pos];
			if(Type != EEnvironmentType::EEnvironmentType_Vein)
			{
				return;
			}
		}
		else if(Cells[Pos] != EEnvironmentType::EEnvironmentType_Empty)
		{
			return;
		}
		FTransform Transform = FTransform(FVector(Piece.Position.X, Piece.Position.Y, 0.f) * CellSize);
		AActor* Actor = GetWorld()->SpawnActorDeferred<AActor>(BuildingTypes[Piece.Type], Transform);
		if(ABuilding* Building = Cast<ABuilding>(Actor))
		{
			Building->S_SetTeamId(Piece.TeamId);
			Building->M_ShowInfoBar(true, 1.f);
			Building->InitGridValues(this, Piece);
		}
		Actor->FinishSpawning(Transform);
		Cells[Piece.Position.X + Piece.Position.Y * GridDimensions] = Piece.Type;
		//M_UpdateGrid(Piece.Position, EEnvironmentType::EEnvironmentType_Empty);
	}
}

void AGridManager::ClearPiece(const FGridPiece Piece)
{
	FIntVector2 Position = Piece.Position;
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return;
	}

	Cells[Position.X + Position.Y * GridDimensions] = EEnvironmentType::EEnvironmentType_Empty;
	//M_UpdateGrid(Piece.Position, EEnvironmentType::EEnvironmentType_Empty);
}

bool AGridManager::CheckCanPlace(const FGridPiece Piece)
{
	FIntVector2 Position = Piece.Position;
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return false;
	}
	int32 Pos = Position.X + Position.Y * GridDimensions;
	
	if(Cells.Num() < Pos)
	{
		return false;
	}
	if(Piece.Type == EEnvironmentType::EEnvironmentType_Extractor)
	{
		EEnvironmentType Type = Cells[Pos];
		return Type == EEnvironmentType::EEnvironmentType_Vein;
	}
	
	return Cells[Position.X + Position.Y * GridDimensions] == EEnvironmentType::EEnvironmentType_Empty;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	InitGrid();
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AGridManager, Cells);
	// DOREPLIFETIME(AGridManager, VisibleCells);
}

bool AGridManager::CheckVisible(const FVector2D& From, const FVector2D& To, double Angle, TArray<bool>& VisibleCells)
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
	if (Angle > 0.52359877559 && Angle < 1.0471975512)
	{
		Direction = FVector2D(1, -1);
	}
	else if(Angle > 2.0943951024 && Angle < 2.61799387799)
	{
		Direction = FVector2D(-1, -1);
	}
	else if(Angle > 3.66519142919 && Angle < 4.18879020479)
	{
		Direction = FVector2D(-1, 1);
	}
	else if(Angle > 5.23598775598 && Angle < 5.75958653158)
	{
		Direction = FVector2D(1, 1);
	}
	for(int i = 0; i <= SideLength; i++)
	{
		if(IsBlockingVision(FIntVector2(CurrentX, CurrentY), DirectionInt))
		{
			break;
		}
		uint32 index = static_cast<uint32>(CurrentX) + static_cast<uint32>(CurrentY) * GridDimensions;
		VisibleCells[index] = true;
		CurrentX += XIncrement;
		CurrentY += YIncrement;
	}
	return false;
}

bool AGridManager::IsBlockingVision(const FIntVector2& Position, const FIntVector2& Direction)
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

EEnvironmentType& AGridManager::GetCell(const FIntVector2& Position)
{
	if(Position.X < 0 || Position.Y < 0 || Position.X >= GridDimensions || Position.Y >= GridDimensions)
	{
		return Cells[0];
	}
	return Cells[static_cast<int32>(Position.X) + static_cast<int32>(Position.Y) * GridDimensions];
}

bool AGridManager::GetNeighbor(const FIntVector2& Position, const FIntVector2& Direction, EEnvironmentType& OutCell)
{
	FIntVector2 NeighborPosition = FIntVector2(Position.X + Direction.X, Position.Y + Direction.Y);
	if(NeighborPosition.X < 0 || NeighborPosition.Y < 0 || NeighborPosition.X >= GridDimensions || NeighborPosition.Y >= GridDimensions)
	{
		return false;
	}
	OutCell = Cells[NeighborPosition.X + NeighborPosition.Y * GridDimensions];
	return true;
}

bool AGridManager::GetVisibleCell(const TArray<bool>& Arr, const FIntVector2& Position)
{
	return Arr[Position.X + Position.Y * GridDimensions];
}

void AGridManager::ClearAllVisible(TArray<bool>& Arr)
{	
	for(int i = 0; i < Arr.Num(); i++)
	{
		if(Arr[i])
		{
			Arr[i] = false;
		}
	}
}

void AGridManager::DrawVisible(ETeamId TeamId, TArray<bool>& VisibleCells)
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
				for(double Angle = 0.; Angle <= 2. * PI; Angle += 0.01)
				{
					double X = (Actor->GetActorLocation().Y + 100.) / 200.;
					double Y = 127. - (Actor->GetActorLocation().X - 100.) / 200.;
					FVector2D From = FVector2D(X, Y);
					FVector2D To = FVector2D(FMath::Cos(Angle) * 10. + X, FMath::Sin(Angle) * 10 + Y);
					CheckVisible(From, To, Angle, VisibleCells);
				}
			}
		}
	}
}



FVector AGridManager::GetTransformedVector(const FIntVector2& Position)
{
	double X = 127 - Position.Y;
	double Y = Position.X;
	
	return FVector(X, Y, 0.f) * CellSize;
}

bool AGridManager::IsTargetVisibleForTeam(const AActor* Target, ETeamId TeamId)
{
	FIntVector2 TargetPosition = FIntVector2((Target->GetActorLocation().Y + 100) / 200, 127 - (Target->GetActorLocation().X + 100) / 200);
	TArray<bool>& Arr = GetVisibleCellsArray(TeamId);
	return Arr[TargetPosition.X + TargetPosition.Y * GridDimensions];
}

void AGridManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		TArray<bool>& Red = GetVisibleCellsArray(ETeamId::Red);
		ClearAllVisible(Red);
		DrawVisible(ETeamId::Red, Red);
		TArray<bool>& Blue = GetVisibleCellsArray(ETeamId::Blue);
		ClearAllVisible(Blue);
		DrawVisible(ETeamId::Blue, Blue);
	}
	else
	{
		ETeamId TeamId = UUtil::GetLocalPlayerTeamId(this);
		TArray<bool>& VisibleCells = GetVisibleCellsArray(TeamId);
		if(TeamId == ETeamId::Neutral || VisibleCells.Num() == 0)
		{
			return;
		}
		ClearAllVisible(VisibleCells);
		DrawVisible(TeamId, VisibleCells);
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

//void AGridManager::M_UpdateGrid_Implementation(const FIntVector2& Position, EEnvironmentType EnvironmentType) {}
