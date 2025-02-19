// Fill out your copyright notice in the Description page of Project Settings.


#include "GridManager.h"

#include "Cell.h"


AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

#pragma region UnrealFunctions
void AGridManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Draw each CellPos debugLines
	for (const TTuple<UE::Math::TVector2<double>, UCell*>& Cell : Cells)
	{
		if(Cell.Value)
		{
			Cell.Value->DrawCellLinesDebug(GridDebugLinesColor, CellSize);
		}
	}

	////Draw horizontal Grid debugLine
	DrawDebugLine(GetWorld(), FVector(GridOrigin.X, 0.f, Columns * CellSize * -1.f + GridOrigin.Z), FVector(Rows * CellSize + GridOrigin.X, 0.f, Columns * CellSize * -1.f + GridOrigin.Z), GridDebugLinesColor, false, -1.f, 0, 3);
	////Draw vertical Grid debugLine
	DrawDebugLine(GetWorld(), FVector(Rows * CellSize + GridOrigin.X, 0.f, Columns * CellSize * -1.f + GridOrigin.Z), FVector(Rows * CellSize + GridOrigin.X, 0.f,  GridOrigin.Z), GridDebugLinesColor, false, -1.f, 0, 3);

	//If set, them draw the StartCell Pos debug
	if(StartCell)
	{
		StartCell->DrawPositionDebug(FColor::Green, CellSize, CellSize / 2.f);
	}

	//If set, them draw the DestinationCell Pos debug
	if(DestinationCell)
	{
		DestinationCell->DrawPositionDebug(FColor::Cyan, CellSize, CellSize / 2.f);
	}

	//If set, them draw each Path CellPos debug
	if(!Path.IsEmpty())
	{
		for (const UCell* PathCell : Path)
		{
			PathCell->DrawPositionDebug(FColor::Magenta, CellSize, CellSize / 4.f);
		}
	}
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	//Read TXT from the Source folder
	bool bReadSuccess;
	FString InfoMessage;
	FString LoadedGridText = ReadStringFromFile(FString("FicheroGrid.txt"), bReadSuccess, InfoMessage);

	//If ReadStringFromFile was successful then create the Grid 
	if(bReadSuccess)
	{
		//Set some class variables
		GridOrigin.X = GetActorLocation().X;
		GridOrigin.Z = GetActorLocation().Z;
		
		int CurrentRow = 0;
		int CurrentColumn = 0;
		
		//Loop through every character in the TXT
		for (const TCHAR& Letter : LoadedGridText)
		{
			//If the character is '\n' then jump to the next column
			if(Letter == '\n')
			{
				++CurrentColumn;
				CurrentRow = 0;
			}
			else
			{
				//Define the data of the Cell according to the current character
				if(Letter != '\r')
				{
					if(UCell* NewCell = NewObject<UCell>(this, UCell::StaticClass()))
					{
						Cells.Add(FVector2D(CurrentRow, CurrentColumn), NewCell);
						FVector2D CellPos = FVector2D(CurrentRow * CellSize + GridOrigin.X, CurrentColumn * CellSize * -1.f + GridOrigin.Z);
						NewCell->SetPosition(CellPos);
						NewCell->SetIsNavigable(Letter!='#');
						NewCell->SetCellData(CellsData[FString(1, &Letter)]);
						++CurrentRow;
					}
				}
			}
		}

		//Set some class variables
		Rows = CurrentRow;
		Columns = CurrentColumn + 1;

		//Define neighbors
		SetCellsNeighbors();	
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, InfoMessage);
	}
}

FString AGridManager::ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage)
{
	FilePath = *(FPaths::GameSourceDir() + FilePath);
	if(!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("ReadStringFromFile failed - File doesn´t exist - '%s'"), *FilePath);
		return "";
	}

	FString RetString = "";
	
	if(!FFileHelper::LoadFileToString(RetString, *FilePath))
	{
		bOutSuccess = false;
		OutInfoMessage = FString::Printf(TEXT("ReadStringFromFile failed - Is this a text file? - '%s'"), *FilePath);
		return "";
	}

	bOutSuccess = true;
	OutInfoMessage = FString::Printf(TEXT("ReadStringFromFile succeded"));
	return RetString;
}

#pragma endregion UnrealFunctions

#pragma region CellsFunctions
UCell* AGridManager::GetCellByGridPos(const int CellX, const int CellY) const
{
	if(CellX < 0 || CellY < 0 || CellX > Rows-1 || CellY > Columns-1) return nullptr;
	
	const int CellIndex = CellX * Columns + CellY;
	if(CellIndex > -1 && CellIndex < Cells.Num())
	{
		return Cells[FVector2D(CellX, CellY)];
	}
	
	return nullptr;
}

int AGridManager::GetDistanceToCell(const UCell* Cell1, const UCell* Cell2) const
{
	int DeltaX = FMath::Abs(Cell1->GetCellPos().X - Cell2->GetCellPos().X);
	int DeltaY = FMath::Abs(Cell1->GetCellPos().Y - Cell2->GetCellPos().Y);

	int MaxValue = FMath::Max(DeltaX, DeltaY);
	int MinValue = FMath::Min(DeltaX, DeltaY);

	return 1.4f * MinValue + (MaxValue - MinValue);
}

bool AGridManager::CheckIfBetweenNonNavigables(const UCell* Cell1, const UCell* Cell2) const
{
	const FVector2D Cell1GridPos = GetCellGridPos(Cell1);
	const FVector2D Cell2GridPos = GetCellGridPos(Cell2);

	if(Cell1GridPos == FVector2D(-1, -1) || Cell2GridPos == FVector2D(-1, -1))
	{
		return false;
	}

	FVector2D CheckCell1Pos = FVector2D(Cell1GridPos.X, Cell2GridPos.Y);
	FVector2D CheckCell2Pos = FVector2D(Cell2GridPos.X, Cell1GridPos.Y);

	UCell* CheckCell1 = GetCellByGridPos(CheckCell1Pos.X, CheckCell1Pos.Y);
	UCell* CheckCell2 = GetCellByGridPos(CheckCell2Pos.X, CheckCell2Pos.Y);

	if(CheckCell1 == nullptr || CheckCell2 == nullptr)
	{
		return false;
	}

	if(!CheckCell1->GetIsNavigable() && !CheckCell2->GetIsNavigable())
	{
		return true;
	}
	
	return false;
}

FVector2D AGridManager::GetCellGridPos(const UCell* Cell) const
{
	if(Cell == nullptr) return FVector2D(-1, -1);

	for (const TTuple<UE::Math::TVector2<double>, UCell*>& CellItr : Cells)
	{
		if(CellItr.Value == Cell)
		{
			return CellItr.Key;
		}
	}

	return FVector2D(-1, -1);
}

void AGridManager::SetPathPosition(const FVector& WorldLocation, EPathPosition PathPosition)
{
	if(WorldLocation.X < GridOrigin.X || WorldLocation.X > GridOrigin.X + Rows * CellSize) return;
	
	if(WorldLocation.Z > GridOrigin.Z || WorldLocation.Z < GridOrigin.Z - Columns * CellSize) return;
	
	const int CellX = FMath::Abs(WorldLocation.X - GridOrigin.X) / CellSize;
	const int CellY = FMath::Abs(WorldLocation.Z - GridOrigin.Z) / CellSize;

	if(UCell* ResultCell = GetCellByGridPos(CellX, CellY))
	{
		if(!ResultCell->GetIsNavigable()) return;
		
		if(PathPosition == EPathPosition::Start)
		{
			StartCell = ResultCell;
			if(DestinationCell!=nullptr)
			{
				CalculatePath();
			}
		}
		else
		{
			DestinationCell = ResultCell;
			if(StartCell!=nullptr)
			{
				CalculatePath();
			}
		}
	}
}

void AGridManager::SetCellsNeighbors()
{
	for (const TTuple<UE::Math::TVector2<double>, UCell*>& Cell : Cells)
	{
		for (int x = -1; x < 2; ++x)
		{
			for (int y = -1; y < 2; ++y)
			{
				if(x == 0 && y == 0) continue;

				if(UCell* PossibleNeighbor = GetCellByGridPos(Cell.Key.X + x, Cell.Key.Y + y))
				{
					Cell.Value->AddNeighbor(PossibleNeighbor);
				}
			}
		}	
	}
	
	// for (TTuple<UE::Math::TVector2<double>, UCell*>& Cell : Cells)
	// {
	// 	const FVector2D& CellPos = Cell.Key;
	// 	if(CellPos.X - 1 >= 0)
	// 	{
	// 		Cell.Value->AddNeighbor(GetCellByGridPos(CellPos.X-1, CellPos.Y));
	// 	}
	// 	if(CellPos.X + 1 < Rows)
	// 	{
	// 		Cell.Value->AddNeighbor(GetCellByGridPos(CellPos.X+1, CellPos.Y));
	// 	}
	// 	if(CellPos.Y - 1 >= 0)
	// 	{
	// 		Cell.Value->AddNeighbor(GetCellByGridPos(CellPos.X, CellPos.Y-1));
	// 	}
	// 	if(CellPos.Y + 1 < Columns)
	// 	{
	// 		Cell.Value->AddNeighbor(GetCellByGridPos(CellPos.X, CellPos.Y+1));
	// 	}
	// }
}
#pragma endregion CellsFunctions

void AGridManager::CalculatePath()
{
	//Remove the previous path
	Path.Empty();
	
	TArray<UCell*> SearchingCells;
	TArray<UCell*> CheckedCells;

	SearchingCells.Add(StartCell);

	while (!SearchingCells.IsEmpty())
	{
		//Try to get the most promising cell
		UCell* CheckCell = SearchingCells[0];
		for (int OpenCellIndex = 0; OpenCellIndex < SearchingCells.Num(); ++OpenCellIndex)
		{
			//Get the Open Cell with the lowest F (If some cells have the same F, then the cell with the lowest H)
			if(SearchingCells[OpenCellIndex]->GetF() < CheckCell->GetF() || (SearchingCells[OpenCellIndex]->GetF() == CheckCell->GetF() && SearchingCells[OpenCellIndex]->GetH() < CheckCell->GetH()))
			{
				CheckCell = SearchingCells[OpenCellIndex];
			}
		}

		//Check if the CheckCell is already the destination Cell
		if(CheckCell == DestinationCell)
		{
			UCell* CurrentPathCell = DestinationCell;
			while(CurrentPathCell != StartCell)
			{
				Path.Add(CurrentPathCell);
				CurrentPathCell = CurrentPathCell->GetLinkedCell();
			}
			Path.Add(StartCell);
			
			return;
		}
		
		//Move the most promising Cell from Searching to Checked
		SearchingCells.Remove(CheckCell);
		CheckedCells.Add(CheckCell);

		//Loop through CheckCells´s Neighbors
		for (UCell* Neighbor : CheckCell->GetNeighbors())
		{
			//Check if the neighbor has not been already checked
			if(!CheckedCells.Contains(Neighbor) && Neighbor->GetIsNavigable() && !CheckIfBetweenNonNavigables(CheckCell, Neighbor))
			{
				const bool InSearch = SearchingCells.Contains(Neighbor);
				
				const int NewG = CheckCell->GetG() + GetDistanceToCell(CheckCell, Neighbor) + Neighbor->GetCellCost();

				//If the neighbor is not InSearch or if the NewG is less than his currentG then Set his G and LinkedCell
				if(!InSearch || NewG < Neighbor->GetG())
				{
					Neighbor->SetG(NewG);
					Neighbor->SetLinkedCell(CheckCell);

					//If the neighbor is not InSearch then Add it to SearchingCells and Set his H
					if(!InSearch)
					{
						Neighbor->SetH(GetDistanceToCell(Neighbor, DestinationCell));
						SearchingCells.Add(Neighbor);
					}
				}
			}
		}
	}
}