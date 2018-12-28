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

	SetupAsteroid(FVector(10, 0, 0), FVector(0, 0, 0), 1., 100);

	Tags.Add(FName("doesDamage"));	
}

void AAsteroid::OnConstruction(const FTransform& Transform)
{
	mAsteroidMeshComponent->AddImpulse(mInitialVelocity,NAME_None, true);

	mAsteroidMeshComponent->AddAngularImpulse(mInitialAngularVelocity, NAME_None, true);
	
	mAsteroidMeshComponent->SetWorldScale3D(FVector(mScale));

	mAsteroidMeshComponent->SetEnableGravity(false);
}

void AAsteroid::SetupAsteroid(FVector initialVelocity, FVector initialAngularVelocity, float scale, float hitPoints)
{
	mInitialVelocity = initialVelocity;
	mInitialAngularVelocity = initialAngularVelocity;
	mScale = scale;
	mHitPoints = hitPoints;
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
	mHitPoints -= damage;

	logf(mHitPoints);

	if (mHitPoints <= 0.)
	{
		if (mScale > .6f)
		{
			UWorld* world = GetWorld();
					   
			UClass* thisClass = this->GetClass();

			FTransform thisTransform = GetActorTransform();

			AAsteroid* piece0 = world->SpawnActorDeferred<AAsteroid>(thisClass, thisTransform);
			AAsteroid* piece1 = world->SpawnActorDeferred<AAsteroid>(thisClass, thisTransform);

			FVector currentVel = mAsteroidMeshComponent->GetComponentVelocity();
			FVector currentAngVel = mAsteroidMeshComponent->GetPhysicsAngularVelocity();

			piece0->SetupAsteroid(currentVel.RotateAngleAxis(-45, FVector(0, 0, 1)), currentAngVel.RotateAngleAxis(-45, FVector(1, 1, 1))/100, mScale*.8, mHitPoints*.8);
			piece1->SetupAsteroid(currentVel.RotateAngleAxis( 45, FVector(0, 0, 1)), currentAngVel.RotateAngleAxis(-45, FVector(1, 1, 1))/100, mScale*.8, mHitPoints*.8);

			

			piece0->FinishSpawning(thisTransform);
			piece1->FinishSpawning(thisTransform);
		}

		if (mExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, mExplosionSound, GetActorLocation());
		}
		
		if (instigator)
		{

			IDamageInterface* dmgAgent = Cast<IDamageInterface>(instigator);
			if (dmgAgent)
			{
				dmgAgent->ReceiveDeathNotification_Implementation(100);
			}

		}
		Destroy();
	}

}