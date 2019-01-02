// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"
#include "portifolioProjectile.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "DebugUtils.h"

// Sets default values
ATarget::ATarget()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<USoundBase> explosionSoundAsset(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));
	mExplosionSound = explosionSoundAsset.Object;

	mMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	RootComponent = mMeshComponent;

	mMeshComponent->BodyInstance.SetCollisionProfileName("Target");
	mMeshComponent->SetSimulatePhysics(true);
	mMeshComponent->SetEnableGravity(false);
	mMeshComponent->BodyInstance.bLockZTranslation = true;
	mMeshComponent->SetNotifyRigidBodyCollision(true);

	Tags.Add(FName("doesDamage"));
}

// Called when the game starts or when spawned
void ATarget::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATarget::ReceiveDamage_Implementation(APawn* instigator, float damage)
{
	mHitPoints -= damage;

	if (mHitPoints <= 0.)
	{
		if (IsValid(mExplosionSound))
		{
			UGameplayStatics::PlaySoundAtLocation(this, mExplosionSound, GetActorLocation());
		}

		if (IsValid(instigator))
		{
			if ( instigator->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) )
			{
				// giving points to damage instigator
				IDamageInterface::Execute_ReceiveDeathNotification(instigator, mScoreValue);
			}
		}
		// Spawner will be in charge of destroying it
		mNotifyDeathDelegate.Broadcast(this);
	}

}