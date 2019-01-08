// Copyright 1998-2018 Epic Games, Inc. All Rights Reserve

#include "portifolioProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "DamageInterface.h"

AportifolioProjectile::AportifolioProjectile()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickProjectile.TwinStickProjectile"));

	// Create mesh component for the projectile sphere
	mProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	mProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	mProjectileMesh->SetupAttachment(RootComponent);
	mProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	mProjectileMesh->OnComponentHit.AddDynamic(this, &AportifolioProjectile::OnHit);		// set up a notification for when this component hits something
	RootComponent = mProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	mProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	mProjectileMovement->UpdatedComponent = mProjectileMesh;
	mProjectileMovement->InitialSpeed = 3000.f;
	mProjectileMovement->MaxSpeed = 3000.f;
	mProjectileMovement->bRotationFollowsVelocity = true;
	mProjectileMovement->bShouldBounce = false;
	mProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	mDamage = 1.f;

	Tags.Add(FName("wrappable"));
}

#include "DebugUtils.h"

void AportifolioProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (IsValid(OtherActor))
	{

		// apply damage to target through damage interface
		if ( OtherActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass()) 
			&& IsValid(this->Instigator) 
			&& OtherActor != this->Instigator) // TODO: Maybe allow this in a debuff?
		{
			// Chose execute version of ReceiveDamage because it is also called on blueprint
			IDamageInterface::Execute_ReceiveDamage(OtherActor, this->Instigator, mDamage);
		}

		// Only add impulse and destroy projectile if we hit a physics
		if((OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		}


		AportifolioProjectile* otherProj = Cast<AportifolioProjectile>(OtherActor);

		// we wanna avoid player destroying its own projectiles
		if (IsValid(otherProj)
			&& otherProj->Instigator == this->Instigator)
		{
			// Do nothing
		}
		else
		{
			Destroy();
		}
	}

}