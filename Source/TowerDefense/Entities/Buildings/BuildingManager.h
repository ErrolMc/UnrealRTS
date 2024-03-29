﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "GameFramework/Actor.h"
#include "TowerDefense/Interfaces/IEntityManager.h"
#include "TowerDefense/Interfaces/IHUD.h"
#include "TowerDefense/UI/HUDPanels/IHUDPanelBuildings.h"

#include "BuildingManager.generated.h"

UCLASS(ABSTRACT)
class TOWERDEFENSE_API ABuildingManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY() APlayerController* playerController;
	TScriptInterface<IHUDInterface> hud;
	TScriptInterface<IEntityManagerInterface> entityManager;

	TScriptInterface<IHUDPanelBuildingsInterface> buildingsHudPanel;
	TWeakObjectPtr<ABuildingBase> selectedBuilding;
	bool hudPanelEnabled = false;

	void OnLeftClickDown();
	
public:
	ABuildingManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildingManager")
	TArray<TEnumAsByte<EObjectTypeQuery>> CollisionMask;
	
	UFUNCTION(BlueprintCallable)
	void Setup(TScriptInterface<IHUDInterface> _hud, TScriptInterface<IEntityManagerInterface> _entityManager, APlayerController* _playerController);
};
