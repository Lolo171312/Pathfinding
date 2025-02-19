// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cell.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class UCell;

UENUM(BlueprintType)
enum EPathPosition
{
	Start,
	End
};

USTRUCT(BlueprintType)
struct FCellData
{
	GENERATED_BODY()

	FCellData() : CellCost(0), CellDebugColor(FColor::Black)
	{}

	FCellData(const int Cost, const FColor Color) : CellCost(Cost), CellDebugColor(Color)
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CellCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor CellDebugColor;
};

UCLASS()
class MPV_PRACTICAS_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridManager();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetPathPosition(const FVector& WorldLocation, EPathPosition PathPosition);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FString ReadStringFromFile(FString FilePath, bool& bOutSuccess, FString& OutInfoMessage);
	
	void SetCellsNeighbors();
	
	UCell* GetCellByGridPos(const int CellX, const int CellY) const;
	int GetDistanceToCell(const UCell* Cell1, const UCell* Cell2) const;

	bool CheckIfBetweenNonNavigables(const UCell* Cell1, const UCell* Cell2) const;
	FVector2D GetCellGridPos(const UCell* Cell) const;

	void CalculatePath();
	
	int Rows = 5;
	int Columns = 5;
	FVector GridOrigin = FVector::Zero();
	
	UPROPERTY(EditAnywhere, Category="Grid")
	float CellSize = 100.f;
	UPROPERTY(EditAnywhere, Category="GridData")
	TMap<FString, FCellData> CellsData;
	UPROPERTY(EditAnywhere, Category="GridData")
	FColor GridDebugLinesColor;

	UPROPERTY()
	TMap<FVector2D, UCell*> Cells;
	UPROPERTY()
	TArray<UCell*> Path;

	UPROPERTY()
	UCell* StartCell = nullptr;
	UPROPERTY()
	UCell* DestinationCell = nullptr;
};
