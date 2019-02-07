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
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundFire(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));

	if (sAssetSoundFire.Succeeded()) mpSoundFire = sAssetSoundFire.Object;

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
			mShootCoolDown = .5;
			if (mpMID) mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(1, 1, 1, 1)));
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(1, 0, 0, 1)));
		}
		break;
		case EGunType::NormalGun:
		{
			mShootCoolDown = .1;
			if (mpMID) mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(0, 0, 1, 1)));
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(1, 0, 0, 1)));
		}
		break;
		case EGunType::DoubleGun:
		{
			mShootCoolDown = .1;
			if (mpMID) mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(1, 1, 0, 1)));
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(1, 0, 0, 1)));
		}
		break;
		case EGunType::FractalGun:
		{
			mShootCoolDown = .2;
			if (mpMID) mpMID->SetVectorParameterValue("color0", FLinearColor(FVector4(0, 1, 0, 1)));
			if (mpMID) mpMID->SetVectorParameterValue("color1", FLinearColor(FVector4(1, 0, 0, 1)));
		}
		break;
		default:
			log("There must be a type...")
		break;
	}

}

void AGun::AttachToPawn(APawn* pPawn, const FTransform& crRelativeT)
{
	// so it won't die like a regular collectable
	SetLifeSpan(0);

	SetOwner(pPawn);
	mpMeshComponent->SetHiddenInGame(true);
	mpMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToActor(pPawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorRelativeTransform(crRelativeT);

	mProjectileCollisionProfile = (Cast<AAsteroidShip>(pPawn)) ? "PlayerProjectile" : "TargetProjectile";
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
			AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			pProj->SetDamage(80);
			pProj->GetProjectileMovement()->MaxSpeed = 1500;
			pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
			pProj->GetProjectileMesh()->SetWorldScale3D(FVector(3.));
		}
		break;

		case EGunType::NormalGun:
		{
			AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			pProj->SetDamage(50);
			pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::DoubleGun:
		{
			AProjectile* pProj = GetWorld()->SpawnActor<AProjectile>(GetActorLocation() + 15*GetActorRightVector(), GetActorRotation(), sParams);
			AProjectile* pProj2 = GetWorld()->SpawnActor<AProjectile>(GetActorLocation() - 15* GetActorRightVector(), GetActorRotation(), sParams);

			pProj->SetDamage(40);
			pProj2->SetDamage(40);

			pProj->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
			pProj2->GetProjectileMesh()->SetCollisionProfileName(mProjectileCollisionProfile);
		}
		break;

		case EGunType::FractalGun:
		{
			AFractalProjectile* pProj = GetWorld()->SpawnActor<AFractalProjectile>(GetActorLocation(), GetActorRotation(), sParams);

			int32 numIterations = 4;
			float lifeSpan = .9f;

			pProj->Setup(lifeSpan/(numIterations+1), 100, GetActorForwardVector()*2000.f, numIterations, 2, mProjectileCollisionProfile);
		}
		break;

		default:
		break;
		}

		if (mpSoundFire != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, mpSoundFire, GetActorLocation());
		}

		mCanShoot = false;

		// It won't be paused so we don't need to keep the reference
		FTimerHandle th;

		GetWorld()->GetTimerManager().SetTimer(th, this, &AGun::EnableShooting, mShootCoolDown);
	}
}


void AGun::EnableShooting()
{
	mCanShoot = true;
}