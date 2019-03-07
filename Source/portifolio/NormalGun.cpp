// Fill out your copyright notice in the Description page of Project Settings.

#include "NormalGun.h"
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"
#include "DebugUtils.h"

ANormalGun::ANormalGun()
{
	mShootCoolDown = .2;
}

void ANormalGun::Shoot()
{
	if(mCanShoot)
	{
		// Handle sound and cooldown
		Super::Shoot();

		static FActorSpawnParameters sParams;
		sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		sParams.Instigator = Cast<APawn>(GetOwner());

		AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

		pProj->SetDamage(50);
		pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
	}
}