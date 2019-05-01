// Fill out your copyright notice in the Description page of Project Settings.

#include "FractalGun.h"
#include "FractalProjectile.h"
#include "Engine/World.h"


AFractalGun::AFractalGun()
{
	mShootCoolDown = .2;
}

void AFractalGun::Shoot_Implementation()
{
	if (mCanShoot)
	{
		// Handle sound and cooldown
		Super::Shoot_Implementation();
		static FActorSpawnParameters sParams;
		sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		sParams.Instigator = Cast<APawn>(GetOwner());

		AFractalProjectile* pProj = GetWorld()->SpawnActor<AFractalProjectile>(GetActorLocation(), GetActorRotation(), sParams);

		int32 numIterations = 4;
		float lifeSpan = .9f;

		pProj->Setup(lifeSpan / (numIterations + 1), 100, GetActorForwardVector()*2000.f, numIterations, 2, mProjectileCollisionProfile);
	}
}