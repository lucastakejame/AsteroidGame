// Copyright 1998-2018 Epic Games, Inc. All Rights Reserve

#include "Projectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "DamageInterface.h"

AProjectile::AProjectile()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Asteroids/Art/Projectile/SM_Projectile.SM_Projectile"));

	// Create mesh component for the projectile sphere
	mpMeshComponentProjectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	mpMeshComponentProjectile->SetStaticMesh(ProjectileMeshAsset.Object);
	mpMeshComponentProjectile->SetupAttachment(RootComponent);
	mpMeshComponentProjectile->BodyInstance.SetCollisionProfileName("TargetProjectile");
	mpMeshComponentProjectile->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something
	mpMeshComponentProjectile->SetNotifyRigidBodyCollision(true);
	RootComponent = mpMeshComponentProjectile;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	mpProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	mpProjectileMovementComponent->UpdatedComponent = mpMeshComponentProjectile;
	mpProjectileMovementComponent->InitialSpeed = 3000.f;
	mpProjectileMovementComponent->MaxSpeed = 3000.f;
	mpProjectileMovementComponent->bRotationFollowsVelocity = true;
	mpProjectileMovementComponent->bShouldBounce = false;
	mpProjectileMovementComponent->ProjectileGravityScale = 0.f; // No gravity

	mpMeshComponentProjectile->SetAllUseCCD(true);

	// Die after 1 second by default
	InitialLifeSpan = 1.0f;

	mDamage = 1.f;

	Tags.Add(FName("wrappable"));
}

void AProjectile::OnHit(UPrimitiveComponent* pHitComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, FVector normalImpulse, const FHitResult& crHit)
{
	if (IsValid(pOtherActor))
	{

		// apply damage to target through damage interface
		if ( pOtherActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) // this is to also consider blueprints
			&& IsValid(this->Instigator)
			&& pOtherActor != this->Instigator
			&& pOtherComp->ComponentHasTag("damageable"))
		{
			// Chose execute version of ReceiveDamage because it is also called on blueprint
			IDamageInterface::Execute_ReceiveDamage(pOtherActor, this->Instigator, mDamage);
		}

		// Tried to remove this impulse before but don't know how Unreal is handling the collisions
		// depending on the angle of impact, the projectile would or not change the other component momentum
		// this way is more reliable.
		if ((pOtherActor != this) && (pOtherComp != NULL) && pOtherComp->IsSimulatingPhysics())
		{
			pOtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		}

		// With destroy() here, the pOtherActor can't execute it's Hit handling for this reference will be invalid
		SetLifeSpan(0.00001);

		// Without destroy() this function executes 2 times, so now this solves that
		mpMeshComponentProjectile->SetCollisionProfileName("NoCollision");
		mpMeshComponentProjectile->SetVisibility(false);
	}
}