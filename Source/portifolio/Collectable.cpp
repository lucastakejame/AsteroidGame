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

	mpMeshComponent->SetCollisionProfileName("Collectable");
	mpMeshComponent->SetGenerateOverlapEvents(true);

	mType = ECollectableType::ExtraLife;

}

void ACollectable::OnConstruction(const FTransform& crT)
{
	mpMID = mpMeshComponent->CreateDynamicMaterialInstance(0, mpMeshComponent->GetMaterial(0));
}

void ACollectable::SetCollectableType(ECollectableType type)
{
	mType = type;

	switch (type)
	{
		case ECollectableType::ExtraLife:
		{
			if (mpMID)
			{
				mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(.8, .8, .8, 1)));
				mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(0, 1, 0, 1)));
			}
		}
		break;
		case ECollectableType::ControlBoost:
		{
			if (mpMID)
			{
				mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(.8, .8, .8, 1)));
				mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(0, 0, 1, 1)));
			}
		}
		break;
		case ECollectableType::Gun:
		{
			// Gun will set these parameters on construction
		}
		break;
		default:
		break;
	}
}
