// Fill out your copyright notice in the Description page of Project Settings.

#include "Target.h"
#include "Projectile.h"
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

	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundExplosion(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));
	if(sAssetSoundExplosion.Succeeded()) mpSoundExplosion = sAssetSoundExplosion.Object;

	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	RootComponent = mpMeshComponent;

	mpMeshComponent->BodyInstance.SetCollisionProfileName("Target");
	mpMeshComponent->SetSimulatePhysics(true);

	// we live in z = 0
	mpMeshComponent->SetEnableGravity(false);
	mpMeshComponent->BodyInstance.bLockZTranslation = true;

	mpMeshComponent->SetNotifyRigidBodyCollision(true);

	Tags.Add(FName("doesDamage"));
	Tags.Add(FName("wrappable"));

	mpMeshComponent->ComponentTags.Add("damageable");

	mLimitSpeed = 200.;
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// NOTE(lucas): Hack to avoid situation of asteroids being with velocity 0 on the beginning
	if(mpMeshComponent->GetComponentVelocity().Size() <= 0.1)
	{
		mpMeshComponent->SetPhysicsLinearVelocity(FVector(100,0,0));
	}

	if (mpMeshComponent->GetComponentVelocity().Size() > mLimitSpeed)
	{
		// Guarantee limit speed
		FVector clampSpeed = mpMeshComponent->GetComponentVelocity().GetClampedToMaxSize(mLimitSpeed);
		mpMeshComponent->SetPhysicsLinearVelocity(clampSpeed);
	}
}

void ATarget::ReceiveDamage_Implementation(APawn* pInstigator, float damage)
{
	mHitPoints -= damage;

	if (mHitPoints <= 0.)
	{
		if (IsValid(mpSoundExplosion))
		{
			UGameplayStatics::PlaySoundAtLocation(this, mpSoundExplosion, GetActorLocation());
		}

		if (IsValid(pInstigator))
		{
			if ( pInstigator->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) )
			{
				// The Execute_ prefix includes blueprint versions of this function
				// giving points to damage instigator
				IDamageInterface::Execute_ReceiveDeathNotification(pInstigator, mScoreValue);
			}
		}
		// Spawner will be in charge of destroying it
		mOnDeath.Broadcast(this);

		SetLifeSpan(0.1);
	}
}