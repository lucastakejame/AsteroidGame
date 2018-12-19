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
	mProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));

	mAsteroidMeshComponent->BodyInstance.SetCollisionProfileName("PhysicsBody");
	mAsteroidMeshComponent->SetStaticMesh(asteroidMeshAsset.Object);
	mAsteroidMeshComponent->SetSimulatePhysics(true);
	mAsteroidMeshComponent->SetEnableGravity(false);
	mAsteroidMeshComponent->BodyInstance.bLockZTranslation = true;
	mAsteroidMeshComponent->SetNotifyRigidBodyCollision(true);
	mAsteroidMeshComponent->OnComponentHit.AddDynamic(this, &AAsteroid::OnHit);

	mExplosionSound = explosionSoundAsset.Object;

	mProjectileMovementComponent->UpdatedComponent = mAsteroidMeshComponent;

	SetupAsteroid(FVector(10, 0, 0), 1.);

	Tags.Add(FName("doesDamage"));	
}

void AAsteroid::OnConstruction(const FTransform& Transform)
{
	mProjectileMovementComponent->Velocity = mInitialVelocity;

	mAsteroidMeshComponent->SetWorldScale3D(FVector(mLifeFraction));

	mAsteroidMeshComponent->SetEnableGravity(false);
}

void AAsteroid::SetupAsteroid(FVector initialVelocity, float lifeFraction)
{
	mInitialVelocity = initialVelocity;
	mLifeFraction = lifeFraction;
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

void AAsteroid::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (OtherActor != NULL && Cast<AportifolioProjectile>(OtherActor))
	{

		if (mLifeFraction > .5f)
		{
			UWorld* world = GetWorld();

			UClass* thisClass = this->GetClass();

			FTransform thisTransform = GetActorTransform();

			AAsteroid* piece0 = world->SpawnActorDeferred<AAsteroid>(thisClass, thisTransform);
			AAsteroid* piece1 = world->SpawnActorDeferred<AAsteroid>(thisClass, thisTransform);

			piece0->SetupAsteroid(this->mInitialVelocity.RotateAngleAxis(-45, FVector(0, 0, 1)), mLifeFraction*.8);
			piece1->SetupAsteroid(this->mInitialVelocity.RotateAngleAxis(45, FVector(0, 0, 1)), mLifeFraction*.8);
		
			piece0->FinishSpawning(thisTransform);
			piece1->FinishSpawning(thisTransform);
		}
		
		if (mExplosionSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, mExplosionSound, GetActorLocation());
		}

		Destroy();
	}

}