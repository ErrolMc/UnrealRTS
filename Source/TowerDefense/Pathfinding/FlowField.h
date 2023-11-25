﻿#pragma once

#include "IntVectorTypes.h"
#include "Cell.h"
#include "FlowFieldDebugType.h"
#include "TowerDefense/Utils/Array2D.h"

using namespace UE::Geometry;

class FlowField
{
public:
	TWeakObjectPtr<UWorld> world;

	TSharedPtr<TArray2D<TSharedPtr<Cell>>> grid;
	FVector2i gridSize;
	float cellRadius;
	float cellDiameter;
	TArray<TEnumAsByte<EObjectTypeQuery>> wallCollision;
	FlowFieldDebugType debugType = FlowFieldDebugType::None;

	TSharedPtr<Cell> destinationCell; // must set this to nullptr if clearing the grid otherwise this will keep one of the cells alive

	FlowField();
	void Init(float _cellRadius, FVector2i _gridSize, UWorld* _world, const TArray<TEnumAsByte<EObjectTypeQuery>> _wallCollision);

	~FlowField();

	void CreateCostField();
	void CreateIntegrationField(const TSharedPtr<Cell>& _destinationCell);
	void DrawDebug();
	void SetDebugType(FlowFieldDebugType type);

	TSharedPtr<Cell> GetCellFromWorldPos(FVector worldPos) const;

private:
	TArray<TSharedPtr<Cell>> GetNeighboringCells_Cardinals(const TSharedPtr<Cell>& cell) const;
	void AddNeighbor(TArray<TSharedPtr<Cell>>& neighbors, const int x, const int y) const;
};
