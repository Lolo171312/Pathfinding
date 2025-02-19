// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Cell.generated.h"

struct FCellData;
/**
 * 
 */
UCLASS()
class MPV_PRACTICAS_API UCell : public UObject
{
	GENERATED_BODY()

public:
	void DrawPositionDebug(const FColor& Color, const float CellSize, const float DrawRadius) const;
	void DrawCellLinesDebug(const FColor& DebugLineColor, const float CellSize) const;

	void SetCellData(const FCellData& NewData);

	int GetG() const {return g;}
	int GetH() const {return h;}
	int GetF() const {return f;}
	bool GetIsNavigable() const {return IsNavigable;}
	const TArray<UCell*>& GetNeighbors() const {return Neighbors;}
	UCell* GetLinkedCell() const {return LinkedCell;}
	const FVector2D& GetCellPos() const {return Pos;}
	int GetCellCost() const{return CellCost;}
	
	void SetPosition(const FVector2D& NewPos) {Pos = NewPos;}
	void SetPosition(const float X, const float Y)
	{
		Pos.X = X;
		Pos.Y = Y;
	}

	void SetH(const float NewH);
	void SetG(const float NewG);
	void SetIsNavigable(const bool NewIsNavigable) {IsNavigable = NewIsNavigable;}
	void AddNeighbor(UCell* NewNeighbor);
	void SetLinkedCell(UCell* NewLinkedCell) {LinkedCell = NewLinkedCell;}
	void SetCellCost(const int NewCost) {CellCost = NewCost;}
	
private:
	FVector2D Pos;

	UPROPERTY()
	TArray<UCell*> Neighbors;
	
	float g;
	float h;
	float f;
	bool IsNavigable = true;

	int CellCost = 0;
	FColor DebugColor;
	
	UPROPERTY()
	UCell* LinkedCell;
};
