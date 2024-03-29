﻿#include "BulletBase.h"

#include "TowerDefense/Entities/Enemies/EnemyBase.h"

ABulletBase::ABulletBase()
{
	RootObject = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	this->SetRootComponent(RootObject);
	
	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootObject);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(RootObject);

	ProjectileMovement->InitialSpeed = 1500;
	ProjectileMovement->MaxSpeed = 3000;

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ABulletBase::OnOverlapBegin);
}

void ABulletBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemyBase* enemy = Cast<AEnemyBase>(OtherActor);
	if (enemy != nullptr)
	{
		OnHit(OtherActor->GetActorLocation());
		enemy->OnTakeDamage(damage);
		DestroyBullet();
	}
}

void ABulletBase::DestroyBullet()
{
	UnregisterAndDestroy();
}

void ABulletBase::Fire(AActor* target, float _damage)
{
	damage = _damage;
	Register();
}

EntityType ABulletBase::GetEntityType()
{
	return EntityType::Bullet;
}

bool ABulletBase::IsSelectable() const
{
	return false;
}
