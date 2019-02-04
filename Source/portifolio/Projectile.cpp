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
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	mpProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	mpProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	mpProjectileMesh->SetupAttachment(RootComponent);
	mpProjectileMesh->BodyInstance.SetCollisionProfileName("TargetProjectile");
	mpProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something
	mpProjectileMesh->SetNotifyRigidBodyCollision(true);
	RootComponent = mpProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	mpProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	mpProjectileMovement->UpdatedComponent = mpProjectileMesh;
	mpProjectileMovement->InitialSpeed = 3000.f;
	mpProjectileMovement->MaxSpeed = 3000.f;
	mpProjectileMovement->bRotationFollowsVelocity = true;
	mpProjectileMovement->bShouldBounce = false;
	mpProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	mpProjectileMesh->SetAllUseCCD(true);

	// Die after 1 second by default
	InitialLifeSpan = 1.0f;

	mDamage = 1.f;

	Tags.Add(FName("wrappable"));
}

#include "DebugUtils.h"

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{

		// apply damage to target through damage interface
		if ( OtherActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) // this is to also consider blueprints
			&& IsValid(this->Instigator)
			&& OtherActor != this->Instigator)
		{
			// Chose execute version of ReceiveDamage because it is also called on blueprint
			IDamageInterface::Execute_ReceiveDamage(OtherActor, this->Instigator, mDamage);
		}

		// Tried to remove this impulse before but don't know how Unreal is handling the collisions
		// depending on the angle of impact, the projectile would or not change the other component momentum
		// this way is more reliable.
		if ((OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		}

		// With destroy() here, the OtherActor can't execute it's Hit handling for this reference will be invalid
		SetLifeSpan(0.00001);
		
		// Without destroy() this function executes 2 times, so now this solves that
		mpProjectileMesh->SetCollisionProfileName("NoCollision");
		mpProjectileMesh->SetVisibility(false);
	}
}