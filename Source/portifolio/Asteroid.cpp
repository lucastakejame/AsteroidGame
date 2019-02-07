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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sAssetMeshAsteroid(TEXT("/Game/Asteroids/Art/Asteroids/SM_Asteroid.SM_Asteroid"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sAssetMeshAsteroid1(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac0.SM_AsteroidFrac0"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sAssetMeshAsteroid2(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac1.SM_AsteroidFrac1"));

	if (sAssetMeshAsteroid.Succeeded()) mMeshesPossible.Add(sAssetMeshAsteroid.Object);
	if (sAssetMeshAsteroid1.Succeeded()) mMeshesPossible.Add(sAssetMeshAsteroid1.Object);
	if (sAssetMeshAsteroid2.Succeeded()) mMeshesPossible.Add(sAssetMeshAsteroid2.Object);

	mHitPoints = 100;
	mScoreValue = 100;
}

void AAsteroid::SetupAsteroid(const FAsteroidInfo& crInfo, float hitPoints)
{
	// Choose random mesh from possible meshes
	mpMeshComponent->SetStaticMesh(mMeshesPossible[FMath::Rand() % mMeshesPossible.Num()]);

	// Random orientation
	SetActorRotation(FRotationMatrix(FRotator(FMath::FRand() * 360, FMath::FRand() * 360, FMath::FRand() * 360)).ToQuat());

	mHitPoints = hitPoints;

	mInfo = crInfo;

	if(IsValid(mpMeshComponent))
	{
		mpMeshComponent->AddImpulse(crInfo.velocity, NAME_None, true);

		mpMeshComponent->AddAngularImpulseInDegrees(crInfo.angularVelocity, NAME_None, true);

		mpMeshComponent->SetWorldScale3D(FVector(crInfo.scale));
	}
}

void AAsteroid::ReceiveDamage_Implementation(APawn* pInstigator, float damage)
{
	// Super::ReceiveDamage may use mInfo so it should be updated first
	mInfo.velocity = mpMeshComponent->GetComponentVelocity();
	mInfo.angularVelocity = mpMeshComponent->GetPhysicsAngularVelocityInDegrees();

	Super::ReceiveDamage_Implementation(pInstigator, damage);
}