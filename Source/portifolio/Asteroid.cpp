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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMeshAsset(TEXT("/Game/Asteroids/Art/Asteroids/SM_Asteroid.SM_Asteroid"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMesh1Asset(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac0.SM_AsteroidFrac0"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMesh2Asset(TEXT("/Game/Asteroids/Art/Asteroids/SM_AsteroidFrac1.SM_AsteroidFrac1"));

	switch (FMath::Rand()%3)
	{
	case 0:
		mMeshComponent->SetStaticMesh(asteroidMeshAsset.Object);
	break;
	case 1:
		mMeshComponent->SetStaticMesh(asteroidMesh1Asset.Object);
	break;
	case 2:
		mMeshComponent->SetStaticMesh(asteroidMesh2Asset.Object);
	break;
	default:
	break;
	}

	SetActorRotation(FRotationMatrix(FRotator(FMath::FRand() * 360, FMath::FRand() * 360, FMath::FRand() * 360)).ToQuat());

	mHitPoints = 100;
	mScoreValue = 100;
}

void AAsteroid::SetupAsteroid(FAsteroidInfo info, float hitPoints)
{
	mHitPoints = hitPoints;

	mInfo = info;

	if(IsValid(mMeshComponent))
	{
		mMeshComponent->AddImpulse(info.velocity, NAME_None, true);

		mMeshComponent->AddAngularImpulseInDegrees(info.angularVelocity, NAME_None, true);

		mMeshComponent->SetWorldScale3D(FVector(info.scale));
	}
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();
}


void AAsteroid::Destroyed()
{
	Super::Destroyed();
	UWorld* w = GetWorld();

	// Change of spawning gun
	if (IsValid(w)
		&& FMath::FRand() <= .15
		&& mInfo.scale < .6)
	{
		FTransform t = FTransform(GetActorRotation(), GetActorLocation());
		w->SpawnActor<AGun>(AGun::StaticClass(), t)->SetType((EGunType) (FMath::Rand()%int32(EGunType::EnumSize)) );

	}
}


// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AAsteroid::ReceiveDamage_Implementation(APawn* instigator, float damage)
{
	mInfo.velocity = mMeshComponent->GetComponentVelocity();
	mInfo.angularVelocity = mMeshComponent->GetPhysicsAngularVelocityInDegrees();

	Super::ReceiveDamage_Implementation(instigator, damage);
}