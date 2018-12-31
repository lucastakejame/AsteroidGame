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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> asteroidMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<USoundBase> explosionSoundAsset(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));

	mAsteroidMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh"));
	RootComponent = mAsteroidMeshComponent;
	
	mAsteroidMeshComponent->BodyInstance.SetCollisionProfileName("Target");
	mAsteroidMeshComponent->SetStaticMesh(asteroidMeshAsset.Object);
	mAsteroidMeshComponent->SetSimulatePhysics(true);
	mAsteroidMeshComponent->SetEnableGravity(false);
	mAsteroidMeshComponent->BodyInstance.bLockZTranslation = true;
	mAsteroidMeshComponent->SetNotifyRigidBodyCollision(true);

	mExplosionSound = explosionSoundAsset.Object;

	Tags.Add(FName("doesDamage"));	
}

void AAsteroid::OnConstruction(const FTransform& Transform)
{}

void AAsteroid::SetupAsteroid(FAsteroidInfo info)
{
	mInfo = info;

	mAsteroidMeshComponent->AddImpulse(info.velocity, NAME_None, true);

	mAsteroidMeshComponent->AddAngularImpulseInRadians(info.angularVelocity, NAME_None, true);

	mAsteroidMeshComponent->SetWorldScale3D(FVector(info.scale));

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
	mInfo.hitPoints -= damage;

	mInfo.velocity = mAsteroidMeshComponent->GetComponentVelocity();
	mInfo.angularVelocity = mAsteroidMeshComponent->GetPhysicsAngularVelocity();
	
	if (mInfo.hitPoints <= 0.)
	{
		if (mExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, mExplosionSound, GetActorLocation());
		}
		
		if (IsValid(instigator))
		{
			IDamageInterface* dmgAgent = Cast<IDamageInterface>(instigator);
			if (dmgAgent)
			{
				// giving points to damage instigator
				dmgAgent->ReceiveDeathNotification_Implementation(100);
			}
		}
		// Spawner will be in charge of destroying it
		mNotifyDeathDelegate.Broadcast(this);
	}

}