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

void ANormalGun::Shoot_Implementation()
{
	if(mCanShoot)
	{
		// Handle sound and cooldown
		// This SHOULD be _Implementation version, otherwise we get an infinite loop (at least for BlueprintNativeEvents)
		// Other places calling Shoot should call Shoot() instead
		Super::Shoot_Implementation();

		static FActorSpawnParameters sParams;
		sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		sParams.Instigator = Cast<APawn>(GetOwner());

		AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

		pProj->SetDamage(50);
		pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
	}
}