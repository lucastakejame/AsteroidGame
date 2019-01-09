 // Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "portifolioProjectile.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AsteroidShip.h"

#include "DebugUtils.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<USoundBase> fireSoundAsset(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialAsset(TEXT("/Game/Asteroids/Art/Gun/M_Gun.M_Gun"));

	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = mpMeshComponent;

	if (meshAsset.Succeeded())
	{
		mpMeshComponent->SetStaticMesh(meshAsset.Object);
		if (materialAsset.Succeeded())
		{
			mpMeshComponent->SetMaterial(0, materialAsset.Object);
		}
	}
	if (fireSoundAsset.Succeeded()) mpFireSound = fireSoundAsset.Object;

	mpMeshComponent->SetCollisionProfileName("Collectable");
	mpMeshComponent->SetGenerateOverlapEvents(true);

	mCanShoot = true;
	mProjectileCollisionProfile = "TargetProjectile";
	SetType(EGunType::NormalGun);
}

void AGun::SetType(const EGunType type)
{
	mGunType = type;
	switch (type)
	{
		case EGunType::NormalGun:
		{
			mShootPeriod = .1;
		}
		break;
		case EGunType::SlowGun:
		{
			mShootPeriod = .5;
		}
		break;
	}
}

void AGun::AttachToPawn(APawn* pawn, FTransform relativeT)
{
	SetOwner(pawn);
	mpMeshComponent->SetHiddenInGame(true);
	mpMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AttachToActor(pawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorRelativeTransform(relativeT);

	mProjectileCollisionProfile = (Cast<AAsteroidShip>(pawn)) ? "PlayerProjectile" : "TargetProjectile";
}

void AGun::Shoot()
{
	static FActorSpawnParameters sParams;
	sParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	sParams.Instigator = Cast<APawn>(GetOwner());
	check(sParams.Instigator != nullptr);

	if (mCanShoot)
	{
		switch (mGunType)
		{
		case EGunType::SlowGun:
		{
			AportifolioProjectile* projectile = mpWorld->SpawnActor<AportifolioProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			projectile->SetDamage(50);
			projectile->GetProjectileMovement()->MaxSpeed = 1500;
			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::NormalGun:
		{
			AportifolioProjectile* projectile = mpWorld->SpawnActor<AportifolioProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			projectile->SetDamage(50);
			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::DoubleGun:
		{
			AportifolioProjectile* projectile = mpWorld->SpawnActor<AportifolioProjectile>(GetActorLocation() + 15*GetActorRightVector(), GetActorRotation(), sParams);
			AportifolioProjectile* projectile2 = mpWorld->SpawnActor<AportifolioProjectile>(GetActorLocation() - 15* GetActorRightVector(), GetActorRotation(), sParams);

			projectile->SetDamage(40);
			projectile2->SetDamage(40);

			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
			projectile2->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		default:
		break;
		}

		if (mpFireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, mpFireSound, GetActorLocation());
		}

		mCanShoot = false;

		// It won't be paused so we don't need to keep the reference
		FTimerHandle th;

		mpWorld->GetTimerManager().SetTimer(th, this, &AGun::EnableShooting, mShootPeriod);
	}
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	mpWorld = GetWorld();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGun::EnableShooting()
{
	mCanShoot = true;
}