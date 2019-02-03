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

	static ConstructorHelpers::FObjectFinder<USoundBase> explosionSoundAsset(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));
	mpSoundExplosion = explosionSoundAsset.Object;

	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TargetMesh"));
	RootComponent = mpMeshComponent;

	mpMeshComponent->BodyInstance.SetCollisionProfileName("Target");
	mpMeshComponent->SetSimulatePhysics(true);
	mpMeshComponent->SetEnableGravity(false);
	mpMeshComponent->BodyInstance.bLockZTranslation = true;

	mpMeshComponent->SetNotifyRigidBodyCollision(true);

	Tags.Add(FName("doesDamage"));
	Tags.Add(FName("wrappable"));

	mLimitSpeed = 200.;
}

// Called every frame
void ATarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Guarantee limit speed
	mpMeshComponent->SetPhysicsLinearVelocity(mpMeshComponent->GetComponentVelocity().GetClampedToMaxSize(mLimitSpeed));
}

void ATarget::ReceiveDamage_Implementation(APawn* instigator, float damage)
{
	mHitPoints -= damage;

	if (mHitPoints <= 0.)
	{
		if (IsValid(mpSoundExplosion))
		{
			UGameplayStatics::PlaySoundAtLocation(this, mpSoundExplosion, GetActorLocation());
		}

		if (IsValid(instigator))
		{
			if ( instigator->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) )
			{
				// The Execute_ prefix includes blueprint versions of this function
				// giving points to damage instigator
				IDamageInterface::Execute_ReceiveDeathNotification(instigator, mScoreValue);
			}
		}
		// Spawner will be in charge of destroying it
		mOnDeath.Broadcast(this);
	}

}