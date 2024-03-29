// Fill out your copyright notice in the Description page of Project Settings.

#include "MouseInteractionBase.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UMouseInteractionBase::Setup(APlayerController* _playerController, TScriptInterface<IEntityManagerInterface> _entityManager)
{
	playerController = _playerController;
	entityManager = _entityManager;
	
	selectionBoxSlot = Cast<UCanvasPanelSlot>(SelectionBox->Slot);
	world = GetWorld();
}

void UMouseInteractionBase::OnUpdate()
{
	const bool isLeftClickDown = playerController->IsInputKeyDown(EKeys::LeftMouseButton);
	const FVector2d mousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(world);
	
	if (isDragging)
	{
		if (!isLeftClickDown)
		{
			isDragging = false;
			SelectionBox->SetVisibility(ESlateVisibility::Hidden);
			return;
		}

		// get the top left and pos delta values to draw the rectangle
		FVector2d topLeft = mousePos;
		if (startDragPos.X < topLeft.X)
			topLeft.X = startDragPos.X;
		if (startDragPos.Y < topLeft.Y)
			topLeft.Y = startDragPos.Y;

		FVector2d posDelta = mousePos - startDragPos;
		posDelta.X = abs(posDelta.X);
		posDelta.Y = abs(posDelta.Y);

		selectionBoxSlot->SetPosition(topLeft);
		selectionBoxSlot->SetSize(posDelta);

		// get bottom right for selection
		FVector2d bottomRight = startDragPos;
		if (mousePos.X > bottomRight.X)
			bottomRight.X = mousePos.X;
		if (mousePos.Y > bottomRight.Y)
			bottomRight.Y = mousePos.Y;

		FVector2d bottomLeft = bottomRight;
		bottomLeft.X = topLeft.X;

		FVector2d topRight = topLeft;
		topLeft.X = bottomRight.X;

		// calculate viewport scale factor
		const FGeometry viewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(world);
		const FVector2d localViewportSize = viewportGeometry.GetLocalSize();
		const FVector2d absoluteViewportSize = viewportGeometry.GetAbsoluteSize();
		const FVector2d scaleFactor = FVector2d(absoluteViewportSize.X / localViewportSize.X, absoluteViewportSize.Y / localViewportSize.Y);

		// convert from screen to world space and then do selection
		const FVector topLeftWorldPos = GetWorldPos(topLeft, scaleFactor);
		const FVector bottomRightWorldPos = GetWorldPos(bottomRight, scaleFactor);
		const FVector bottomLeftWorldPos = GetWorldPos(bottomLeft, scaleFactor);
		const FVector topRightWorldPos = GetWorldPos(topRight, scaleFactor);

		entityManager->UpdateSelectedEntitiesInRange(topLeftWorldPos, bottomRightWorldPos, bottomLeftWorldPos, topRightWorldPos);
	}
	else if (isLeftClickDown)
	{
		startDragPos = mousePos;
		
		selectionBoxSlot->SetPosition(startDragPos);
		
		SelectionBox->SetVisibility(ESlateVisibility::Visible);
		selectionBoxSlot->SetSize(FVector2d(0, 0));
		curDragAnchors = selectionBoxSlot->GetAnchors();

		isDragging = true;
	}
}

FVector UMouseInteractionBase::GetMousePosInWorld() const
{
	return GetWorldPos(UWidgetLayoutLibrary::GetMousePositionOnViewport(world));
}

FVector UMouseInteractionBase::GetWorldPos(FVector2d screenPos) const
{
	const FGeometry viewportGeometry = UWidgetLayoutLibrary::GetViewportWidgetGeometry(world);
	const FVector2d localViewportSize = viewportGeometry.GetLocalSize();
	const FVector2d absoluteViewportSize = viewportGeometry.GetAbsoluteSize();
	const FVector2d scaleFactor = FVector2d(absoluteViewportSize.X / localViewportSize.X, absoluteViewportSize.Y / localViewportSize.Y);

	return GetWorldPos(screenPos, scaleFactor);
}

FVector UMouseInteractionBase::GetWorldPos(FVector2d screenPos, FVector2d scaleFactor) const
{
	const FVector2d absoluteScreenPos = FVector2d(screenPos.X * scaleFactor.X, screenPos.Y * scaleFactor.Y);
	
	FHitResult hit;
	playerController->GetHitResultAtScreenPosition(
		absoluteScreenPos,
		GroundCollision,
		true,
		hit);

	return hit.Location;
}
