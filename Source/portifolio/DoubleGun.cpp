// Fill out your copyright notice in the Description page of Project Settings.

#include "DoubleGun.h"
#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"



ADoubleGun::ADoubleGun()
{
	mShootCoolDown = .2;
}

void ADoubleGun::Shoot_Implementation()
{
	if (mCanShoot)
	{
		// Handle sound and cooldown
		Super::Shoot_Implementation();
		static FActorSpawnParameters sParams;
		sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		sParams.Instigator = Cast<APawn>(GetOwner());

		AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation() + 15 * GetActorRightVector(), GetActorRotation(), sParams);
		AProjectile* pProj2 = GetWorld()->SpawnActor<AProjectile>(GetActorLocation() - 15 * GetActorRightVector(), GetActorRotation(), sParams);

		pProj->SetDamage(40);
		pProj2->SetDamage(40);

		pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		pProj2->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
	}
}