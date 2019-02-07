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

private:
// Members

	// While > 0, spawn mNumChilds fractalProjectiles when destroyed
	int32 mIterationsLeft;

	// number of projectiles spawned whem this one dies
	int32 mNumChilds;

	FName mCollisionProfile;

	float mInitialLifeSpan;

	bool mWasHit = false;

public:
// Methods

	// Set all these variables
	void Setup(float lifeSpan, float damage, FVector velocity, int32 iterationsLeft, int32 numChilds, FName collisionProfile);

	// Handle destruction and spawn child fractalProjectiles if there are iterations left
	virtual void Destroyed() override;

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
