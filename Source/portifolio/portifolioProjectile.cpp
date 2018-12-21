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
}

void AportifolioProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != NULL)
	{

		// apply damage to target through damage interface
		IDamageInterface* dmgTarget = Cast<IDamageInterface>(OtherActor);
		if (dmgTarget)
		{
			dmgTarget->ReceiveDamage_Implementation(this->Instigator, mDamage);
		}
		// Only add impulse and destroy projectile if we hit a physics
		if((OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());
		}
	}
	
	Destroy();
}