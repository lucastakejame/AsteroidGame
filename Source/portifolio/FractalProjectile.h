// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FractalProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API AFractalProjectile : public AProjectile
{
	GENERATED_BODY()

public:
// Members

	// How many times this will Spawn, it will spawn n children while mFractalIterations > 0
	int32 mIterationsLeft;

	// number of projectiles spawned whem this one dies
	int32 mNumChilds;
	
	FName mCollisionProfile;

	float mInitialLifeSpan;

	bool mWasHit = false;

// Methods

	void SetupFractalAsteroid(float lifeSpan, float damage, FVector velocity, int32 iterationsLeft, int32 numChilds, FName collisionProfile);

	virtual void Destroyed() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
