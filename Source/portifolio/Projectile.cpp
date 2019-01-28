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
	mProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	mProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	mProjectileMesh->SetupAttachment(RootComponent);
	mProjectileMesh->BodyInstance.SetCollisionProfileName("TargetProjectile");
	mProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something
	mProjectileMesh->SetNotifyRigidBodyCollision(true);
	RootComponent = mProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	mProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	mProjectileMovement->UpdatedComponent = mProjectileMesh;
	mProjectileMovement->InitialSpeed = 3000.f;
	mProjectileMovement->MaxSpeed = 3000.f;
	mProjectileMovement->bRotationFollowsVelocity = true;
	mProjectileMovement->bShouldBounce = false;
	mProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	mProjectileMesh->SetAllUseCCD(true);

	// Die after 1 second by default
	InitialLifeSpan = 1.0f;

	mDamage = 1.f;

	Tags.Add(FName("wrappable"));
}


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

		Destroy();
	}
}