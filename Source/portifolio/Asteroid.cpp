// Fill out your copyright notice in the Description page of Project Settings.

#include "Asteroid.h"
#include "Projectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Gun.h"

#include "DebugUtils.h"

// Sets default values
AAsteroid::AAsteroid()
{
	if (mMeshesPossible.Num() == 0)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMeshAsset(TEXT("/Game/Asteroids/Art/Asteroids/SM_Asteroid.SM_Asteroid"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMesh1Asset(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac0.SM_AsteroidFrac0"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMesh2Asset(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac1.SM_AsteroidFrac1"));

		if (asteroidMeshAsset.Succeeded()) mMeshesPossible.Add(asteroidMeshAsset.Object);
		if (asteroidMesh1Asset.Succeeded()) mMeshesPossible.Add(asteroidMesh1Asset.Object);
		if (asteroidMesh2Asset.Succeeded()) mMeshesPossible.Add(asteroidMesh2Asset.Object);
	}
	
	mHitPoints = 100;
	mScoreValue = 100;
}

void AAsteroid::SetupAsteroid(const FAsteroidInfo& info, float hitPoints)
{

	mpMeshComponent->SetStaticMesh(mMeshesPossible[FMath::Rand() % mMeshesPossible.Num()]);

	SetActorRotation(FRotationMatrix(FRotator(FMath::FRand() * 360, FMath::FRand() * 360, FMath::FRand() * 360)).ToQuat());

	mHitPoints = hitPoints;

	mInfo = info;

	if(IsValid(mpMeshComponent))
	{
		mpMeshComponent->AddImpulse(info.velocity, NAME_None, true);

		mpMeshComponent->AddAngularImpulseInDegrees(info.angularVelocity, NAME_None, true);

		mpMeshComponent->SetWorldScale3D(FVector(info.scale));
	}
}

void AAsteroid::ReceiveDamage_Implementation(APawn* instigator, float damage)
{
	// Super::ReceiveDamage may use mInfo so it should be updated first
	mInfo.velocity = mpMeshComponent->GetComponentVelocity();
	mInfo.angularVelocity = mpMeshComponent->GetPhysicsAngularVelocityInDegrees();

	Super::ReceiveDamage_Implementation(instigator, damage);
}