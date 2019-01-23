 // Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Projectile.h"
#include "FractalProjectile.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AsteroidShip.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "DebugUtils.h"

// Sets default values
AGun::AGun()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> fireSoundAsset(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));

	if (fireSoundAsset.Succeeded()) mpFireSound = fireSoundAsset.Object;

	SetCollectableType(ECollectableType::Gun);
	mCanShoot = true;
	mProjectileCollisionProfile = "TargetProjectile";
	SetGunType(EGunType::NormalGun);
}

void AGun::SetGunType(const EGunType type)
{
	mGunType = type;
	switch (type)
	{
		case EGunType::SlowGun:
		{
			mShootPeriod = .5;
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(.8, .8, .8, 1)));
		}
		break;
		case EGunType::NormalGun:
		{
			mShootPeriod = .1;
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(.2, .2, .8, 1)));
		}
		break;
		case EGunType::DoubleGun:
		{
			mShootPeriod = .1;
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(.8, .8, .2, 1)));
		}
		break;
		case EGunType::FractalGun:
		{
			mShootPeriod = .2;
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(.2, .8, .2, 1)));
		}
		break;
		default:
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
			AProjectile* projectile = mpWorld->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			projectile->SetDamage(80);
			projectile->GetProjectileMovement()->MaxSpeed = 1500;
			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
			projectile->GetProjectileMesh()->SetWorldScale3D(FVector(3.));
		}
		break;

		case EGunType::NormalGun:
		{
			AProjectile* projectile = mpWorld->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			projectile->SetDamage(50);
			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::DoubleGun:
		{
			AProjectile* projectile = mpWorld->SpawnActor<AProjectile>(GetActorLocation() + 15*GetActorRightVector(), GetActorRotation(), sParams);
			AProjectile* projectile2 = mpWorld->SpawnActor<AProjectile>(GetActorLocation() - 15* GetActorRightVector(), GetActorRotation(), sParams);

			projectile->SetDamage(40);
			projectile2->SetDamage(40);

			projectile->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
			projectile2->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::FractalGun:
		{
			AFractalProjectile* projectile = mpWorld->SpawnActor<AFractalProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			int32 numIterations = 4;
			float lifeSpan = .9f;

			projectile->SetupFractalAsteroid(lifeSpan/(numIterations+1), 100, GetActorForwardVector()*2000.f, numIterations, 2, mProjectileCollisionProfile);
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