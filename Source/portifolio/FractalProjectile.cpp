// Fill out your copyright notice in the Description page of Project Settings.

#include "FractalProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

#include "DebugUtils.h"


void AFractalProjectile::Setup(float lifeSpan, float damage, FVector velocity, int32 iterationsLeft, int32 numChilds, FName collisionProfile)
{

	mInitialLifeSpan = lifeSpan;
	SetLifeSpan(lifeSpan);

	SetDamage(damage);

	// Setting direction and speed
	GetProjectileMovement()->Velocity = velocity;
	GetProjectileMovement()->MaxSpeed = velocity.Size();

	mIterationsLeft = iterationsLeft;

	mNumChilds = numChilds;

	mCollisionProfile = collisionProfile;
	GetProjectileMesh()->SetCollisionProfileName(collisionProfile);


}

void AFractalProjectile::Destroyed()
{
	// only spawn it was not hit, otherwise it will be too strong
	if (mIterationsLeft > 0 && !mWasHit)
	{
		for (int i = 0; i < mNumChilds; i++)
		{
			AFractalProjectile* proj = GetWorld()->SpawnActor<AFractalProjectile>(AFractalProjectile::StaticClass(), GetActorTransform());

			proj->Instigator = GetInstigator();

			// Samples 'mNumChilds' values in [0,1]
			float alpha = (float(i)+.5)/mNumChilds;

			FVector vel = GetProjectileMovement()->Velocity.RotateAngleAxis(FMath::Lerp(-20,20,alpha), FVector(0,0,1));

			proj->Setup(mInitialLifeSpan/1.5f, GetDamage()/mNumChilds, vel, mIterationsLeft-1, mNumChilds, mCollisionProfile);
		}
	}

	Super::Destroyed();
}

void AFractalProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	mWasHit = true;
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}