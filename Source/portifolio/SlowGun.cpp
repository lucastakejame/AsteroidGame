// Fill out your copyright notice in the Description page of Project Settings.

#include "SlowGun.h"
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"


ASlowGun::ASlowGun()
{
	mShootCoolDown = .2;
}

void ASlowGun::Shoot()
{
	if (mCanShoot)
	{
		// Handle sound and cooldown
		Super::Shoot();
		static FActorSpawnParameters sParams;
		sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		sParams.Instigator = Cast<APawn>(GetOwner());

		AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

		pProj->SetDamage(80);
		pProj->GetProjectileMovement()->MaxSpeed = 1500;
		pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		pProj->GetProjectileMesh()->SetWorldScale3D(FVector(3.));

	}
}