// Fill out your copyright notice in the Description page of Project Settings.

#include "Collectable.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Gun.h"
#include "DebugUtils.h"

// Sets default values
ACollectable::ACollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sAssetMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundCollected(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> sAssetMaterial(TEXT("/Game/Asteroids/Art/Collectable/M_Collectable.M_Collectable"));

	// Adding component
	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectableMesh"));
	RootComponent = mpMeshComponent;

	if (sAssetMesh.Succeeded()) mpMeshComponent->SetStaticMesh(sAssetMesh.Object);
	if (sAssetMaterial.Succeeded()) mpMeshComponent->SetMaterial(0, sAssetMaterial.Object);

	// Starts with no collision so it won't trigger an overlap when being spawned inside another actor
	// e.g. spawned by AAsteroidShip and equipped right after
	mpMeshComponent->SetCollisionProfileName("NoCollision");
	mpMeshComponent->SetGenerateOverlapEvents(true);

	mType = ECollectableType::ExtraLife;

	InitialLifeSpan = 5.;

}

void ACollectable::BeginPlay()
{
	FTimerHandle th;
	GetWorldTimerManager().SetTimer(th, this, &ACollectable::SetCollisionToCollectable, .1, false);
}

void ACollectable::SetCollisionToCollectable()
{
	// If its not owned by anybody yet, can be collected
	if (!IsValid(GetOwner()))
	{
		mpMeshComponent->SetCollisionProfileName("Collectable");
	}
}