 // Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Projectile.h"
#include "FractalProjectile.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AsteroidShip.h"
#include "EnemyShip.h"

#include "SlowGun.h"
#include "NormalGun.h"
#include "DoubleGun.h"
#include "FractalGun.h"

#include "EngineUtils.h"


#include "DebugUtils.h"

// Sets default values
AGun::AGun()
{
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundFire(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));

	if (sAssetSoundFire.Succeeded()) mpSoundFire = sAssetSoundFire.Object;

	SetCollectableType(ECollectableType::Gun);
	mCanShoot = true;
	mProjectileCollisionProfile = "TargetProjectile";
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


void AGun::ApplyEffect(APawn* pAffectedPawn)
{
	AAsteroidShip* pA = Cast<AAsteroidShip>(pAffectedPawn);
	AEnemyShip* pE = Cast<AEnemyShip>(pAffectedPawn);

	// Note(Lucas): If Other classes use guns, its better to make an interface 
	if (IsValid(pA))
	{
		pA->SetGun(this);
		AttachToPawn(pAffectedPawn, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}
	else if (IsValid(pE))
	{
		pE->SetGun(this);
		AttachToPawn(pAffectedPawn, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}

}


// The child classes call this method if they succeded in shooting
void AGun::Shoot()
{
	if (mpSoundFire) UGameplayStatics::PlaySoundAtLocation(this, mpSoundFire, GetActorLocation());

	mCanShoot = false;
	//It won't be paused so we don't need to access the reference
	FTimerHandle th;
	GetWorldTimerManager().SetTimer(th, this, &AGun::EnableShooting, mShootCoolDown);
}


void AGun::EnableShooting()
{
	mCanShoot = true;
}