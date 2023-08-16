﻿#include "TowerBase.h"

ATowerBase::ATowerBase()
{
	CollisionObject = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere Object"));
}

void ATowerBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());
	AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
	if (enemy == nullptr)
		return;

	const FGuid guid = enemy->GetActorGuid();
	if (EnemiesInRange.Contains(guid))
		return;

	EnemiesInRange.Add(guid, enemy);
}

void ATowerBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
	if (enemy == nullptr)
		return;

	const FGuid guid = enemy->GetActorGuid();
	if (!EnemiesInRange.Contains(guid))
		return;

	EnemiesInRange.Remove(guid);
	if (EnemiesInRange.Num() == 0 || CurrentTarget->GetActorGuid() == guid)
	{
		hasTarget = false;
	}
}

void ATowerBase::BeginPlay()
{
	CollisionObject->SetSphereRadius(Range);
	CollisionObject->SetGenerateOverlapEvents(true);
	CollisionObject->SetHiddenInGame(true);
	CollisionObject->OnComponentBeginOverlap.AddDynamic(this, &ATowerBase::OnOverlapBegin);
	CollisionObject->OnComponentEndOverlap.AddDynamic(this, &ATowerBase::OnOverlapEnd);

	Super::BeginPlay();
}

bool ATowerBase::HasTarget()
{
	if (hasTarget)
		return true;

	return FindClosestEnemy();
}

bool ATowerBase::FindClosestEnemy()
{
	if (EnemiesInRange.Num() == 0)
		return false;

	FGuid minGuid;
	double minDist = 999999;
	const FVector thisPos = this->GetActorLocation();
	
	for (const TTuple<FGuid, AEnemyBase*> keyVal : EnemiesInRange)
	{
		FVector location = keyVal.Value->GetActorLocation();
		const double dist = UE::Geometry::Distance(thisPos, location);

		if (dist < minDist)
		{
			minDist = dist;
			minGuid = keyVal.Key;
		}
	}

	CurrentTarget = EnemiesInRange[minGuid];
	hasTarget = true;
	
	return true;
}
