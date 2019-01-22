// Fill out your copyright notice in the Description page of Project Settings.

#include "Collectable.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Gun.h"

// Sets default values
ACollectable::ACollectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<USoundBase> collectedSoundAsset(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialAsset(TEXT("/Game/Asteroids/Art/Gun/M_Collectable.M_Collectable"));

	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollectableMesh"));
	RootComponent = mpMeshComponent;
	
	if (meshAsset.Succeeded())
	{
		mpMeshComponent->SetStaticMesh(meshAsset.Object);
		if (materialAsset.Succeeded())
		{
			mpMeshComponent->SetMaterial(0, materialAsset.Object);
			mpMID = mpMeshComponent->CreateDynamicMaterialInstance(0, materialAsset.Object);
		}
		else
		{
			mpMID = nullptr;
		}
	}

	mpMeshComponent->SetCollisionProfileName("Collectable");
	mpMeshComponent->SetGenerateOverlapEvents(true);

	mType = ECollectableType::ExtraLife;
}

void ACollectable::SetCollectableType(ECollectableType type)
{
	mType = type;
	switch (type)
	{
		case ECollectableType::ExtraLife:
		{
			if (mpMID) mpMID->SetVectorParameterValue("color", FLinearColor(FVector4(.8, .2, .2, 1)));
		}
		break;
		case ECollectableType::ControlBoost:
		{
			if (mpMID) mpMID->SetVectorParameterValue("color", FLinearColor(FVector4(.2, .8, .2, 1)));
		}
		break;
		case ECollectableType::Gun:
		{
			if (mpMID) mpMID->SetVectorParameterValue("color", FLinearColor(FVector4(.2, .2, .8, 1)));
		}
		break;
		default:
		break;
	}
}


ACollectable* ACollectable::SpawnRandomCollectable(UWorld* world, FTransform t)
{
	ACollectable* result = nullptr;
	if (IsValid(world))
	{
		uint8 collecType = FMath::Rand()%int32(ECollectableType::EnumSize);

		if (ECollectableType(collecType) == ECollectableType::Gun)
		{
			result = world->SpawnActor<AGun>(AGun::StaticClass(), t);
			((AGun*)result)->SetGunType((EGunType)(FMath::Rand() % int32(EGunType::EnumSize)));
		}
		else
		{
			result = world->SpawnActor<ACollectable>(ACollectable::StaticClass(), t);
			result->SetCollectableType((ECollectableType)collecType);
		}
	}
	return result;
}

