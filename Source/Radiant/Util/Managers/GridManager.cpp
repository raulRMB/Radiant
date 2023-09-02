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
#include "Util/Util.h"

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

void AGridManager::InitGrid()
{
	if(!bSpawnMap)
	{
		return;
	}
	
	VisibleCells.Init(false, Cells.Num());
	
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
		M_UpdateGrid(Piece.Position, EEnvironmentType::EEnvironmentType_Empty);
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
	M_UpdateGrid(Piece.Position, EEnvironmentType::EEnvironmentType_Empty);
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
	
	//UUtil::CheckVisible(this, VisionTexture, RenderTarget, FIntVector2(0, 0));
}

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AGridManager, Cells);
	DOREPLIFETIME(AGridManager, VisibleCells);
}


//
// void AGridManager::CheckVisible(const FVector2D& From, const FVector2D& To)
// {
// 	const double DX = To.X - From.X;
// 	const double DY = To.Y - From.Y;
//
// 	const double SideLength = FMath::Max(FMath::Abs(DX), FMath::Abs(DY));
//
// 	const double XIncrement = DX / SideLength;
// 	const double YIncrement = DY / SideLength;
//
// 	double CurrentX = From.X;
// 	double CurrentY = From.Y;
// 	
// 	for(int i = 0; i <= SideLength; i++)
// 	{
// 		// if(Cells[static_cast<int32>(CurrentX) + static_cast<int32>(CurrentY) * GridDimensions] != EEnvironmentType::EEnvironmentType_Empty)
// 		// {
// 		// 	break;
// 		// }
// 		VisibleCells[static_cast<int32>(CurrentX) + static_cast<int32>(CurrentY) * GridDimensions] = true;
// 		CurrentX += XIncrement;
// 		CurrentY += YIncrement;
// 	}
// }

void AGridManager::CheckVisible(const FIntVector2& Start)
{
	for(int Range = 0; Range < 10; Range++)
	{
		//for(int i = 0; )
	}
}

bool AGridManager::GetVisibleCell(const FIntVector2& Position)
{
	return VisibleCells[Position.X + Position.Y * GridDimensions];
}

void AGridManager::ClearVision()
{	
	for(int i = 0; i < VisibleCells.Num(); i++)
	{
		if(VisibleCells[i])
		{
			VisibleCells[i] = false;
		}
	}
}

void AGridManager::SetVisible()
{
	for(AActor* Actor : VisibleActors)
	{
		
		// CheckVisible(Pos);
	}
}

FVector AGridManager::GetTransformedVector(const FIntVector2& Position)
{
	double X = 127 - Position.Y;
	double Y = Position.X;
	
	return FVector(X, Y, 0.f) * CellSize;
}

void AGridManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(VisibleActors.Num() > 0)
	{
		FIntVector2 Pos = FIntVector2((VisibleActors[0]->GetActorLocation().Y + 100) / 200 , 127 - (VisibleActors[0]->GetActorLocation().X - 100) / 200);
		UUtil::CheckVisible(Cells, RenderTarget, Pos);
	}

	// if(VisibleActors.Num() > 0)
	// {
	// 	FIntVector2 Pos = FIntVector2(FMath::Floor(VisibleActors[0]->GetActorLocation().X / 200), FMath::Floor(VisibleActors[0]->GetActorLocation().Y / 200));
	// 	UUtil::CheckVisible(this, MapTexture, VisionTexture, RenderTarget, Pos);
	// }
	
	// if(HasAuthority())
	// {
	// 	ClearVision();
	// 	SetVisible();
	// }
	// else
	// {
	// 	UpdateTexture(FIntVector2(0, 0));
	// }
}

// void AGridManager::UpdateTexture(const FIntVector2& Pos)
// {
// 	UKismetRenderingLibrary::ClearRenderTarget2D(this, RenderTarget, FLinearColor::Black);
// 	
// 	UCanvas* Canvas;
// 	FDrawToRenderTargetContext Context;
// 	FVector2D Size;
// 	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
// 		this,
// 		RenderTarget,
// 		Canvas,
// 		Size,
// 		Context
// 		);
//
// 	for(int i = 0; i < VisibleCells.Num(); i++)
// 	{	
// 		if(VisibleCells[i])
// 		{
// 			Canvas->DrawTile(VisionTexture, i % GridDimensions, i / GridDimensions, 1, 1, 0, 0, 1, 1, BLEND_Opaque);
// 		}
// 	}
// 	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this, Context);
// }

void AGridManager::M_UpdateGrid_Implementation(const FIntVector2& Position, EEnvironmentType EnvironmentType)
{
	// OnGridUpdate.Broadcast(Position, EnvironmentType);

	
}
