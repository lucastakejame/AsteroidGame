// Fill out your copyright notice in the Description page of Project Settings.

#include "Asteroid.h"
#include "portifolioProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DebugUtils.h"

// Sets default values
AAsteroid::AAsteroid()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));

	mMeshComponent->SetStaticMesh(asteroidMeshAsset.Object);

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