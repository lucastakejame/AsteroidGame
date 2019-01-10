// Fill out your copyright notice in the Description page of Project Settings.

#include "FractalProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

#include "DebugUtils.h"


void AFractalProjectile::SetupFractalAsteroid(float lifeSpan, float damage, FVector velocity, int32 iterationsLeft, int32 numChilds, FName collisionProfile)
{
	mInitialLifeSpan = lifeSpan;
	SetLifeSpan(lifeSpan);

	SetDamage(damage);

	// reorient actor through rotation
	GetProjectileMovement()->Velocity = velocity;

	// SetActorRotation(FRotationMatrix::MakeFromX(velocity.GetSafeNormal()).Rotator());
	GetProjectileMovement()->MaxSpeed = velocity.Size();

	mIterationsLeft = iterationsLeft;

	mNumChilds = numChilds;

	mCollisionProfile = collisionProfile;
	GetProjectileMesh()->SetCollisionProfileName(collisionProfile);

}

void AFractalProjectile::Destroyed()
{
	// only spawn if lifespan ended, otherwise it will be too strong
	if (mIterationsLeft > 0 && GetLifeSpan() == 0.)
	{
		for (int i = 0; i < mNumChilds; i++)
		{
			AFractalProjectile* proj = GetWorld()->SpawnActor<AFractalProjectile>(AFractalProjectile::StaticClass(), GetActorTransform());

			proj->Instigator = GetInstigator();

			float alpha = float(i)/ mInitialLifeSpan;

			FVector vel = GetProjectileMovement()->Velocity.RotateAngleAxis(FMath::Lerp(-1,1,alpha), FVector(0,0,1));

			proj->SetupFractalAsteroid(mInitialLifeSpan, GetDamage()/mNumChilds, vel, mIterationsLeft-1, mNumChilds, mCollisionProfile);
		}
	}

	Super::Destroyed();
}