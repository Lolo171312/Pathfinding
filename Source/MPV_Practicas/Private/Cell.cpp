// Fill out your copyright notice in the Description page of Project Settings.


#include "MPV_Practicas/Public/Cell.h"

#include "GridManager.h"

void UCell::DrawPositionDebug(const FColor& Color, const float CellSize, const float DrawRadius) const
{
	FVector RealPos = FVector(Pos.X, 0.f, Pos.Y);
	RealPos.X+=CellSize/2.f;
	RealPos.Z-=CellSize/2.f;
	DrawDebugSphere(GetWorld(), RealPos, DrawRadius, 8, Color, false, -1.f, 0, 3);
}

void UCell::DrawCellLinesDebug(const FColor& DebugLineColor, const float CellSize) const
{
	DrawDebugLine(GetWorld(), FVector(Pos.X, 0, Pos.Y), FVector(Pos.X + CellSize, 0.f, Pos.Y), DebugLineColor, false, -1.f, 0, 3);
	DrawDebugLine(GetWorld(), FVector(Pos.X, 0, Pos.Y), FVector(Pos.X, 0.f, Pos.Y - CellSize), DebugLineColor, false, -1.f, 0, 3);

	FVector CellCenter = FVector(Pos.X, 0.f, Pos.Y);
	CellCenter.X+=CellSize/2.f;
	CellCenter.Z-=CellSize/2.f;
	DrawDebugSolidBox(GetWorld(), CellCenter, FVector(CellSize/2.2f, 1.f, CellSize/2.2f), DebugColor, false, -1.f, 0);

	//for (const UCell* Neighbor : Neighbors)
	//{
	//	FVector InitialPos = FVector(Pos.X, 0.f, Pos.Y);
	//	InitialPos.X+=CellSize/2.f;
	//	InitialPos.Z-=CellSize/2.f;
	//	FVector EndPos = FVector(Neighbor->Pos.X, 0.f, Neighbor->Pos.Y);
	//	EndPos.X+=CellSize/2.f;
	//	EndPos.Z-=CellSize/2.f;
	//	DrawDebugLine(GetWorld(), InitialPos, EndPos, FColor::Cyan, false, -1, 0, 2);
	//}
}

void UCell::SetCellData(const FCellData& NewData)
{
	SetCellCost(NewData.CellCost);
	DebugColor = NewData.CellDebugColor;
}

void UCell::SetG(const float NewG)
{
	g = NewG;
	f = g + h;
}

void UCell::SetH(const float NewH)
{
	h = NewH;
	f = g + h;
}

void UCell::AddNeighbor(UCell* NewNeighbor)
{
	if(!Neighbors.Contains(NewNeighbor))
	{
		Neighbors.Add(NewNeighbor);
	}
}
