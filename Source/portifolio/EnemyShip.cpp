// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyShip.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Gun.h"
#include "AsteroidShip.h"
#include "DebugUtils.h"

AEnemyShip::AEnemyShip() : ATarget()
{
	PrimaryActorTick.bTickEvenWhenPaused = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> enemyMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> enemyMaterialAsset(TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));
	if(enemyMeshAsset.Succeeded()) mpMeshComponent->SetStaticMesh(enemyMeshAsset.Object);
	if(enemyMaterialAsset.Succeeded())  mpMeshComponent->SetMaterial(0, enemyMaterialAsset.Object);

	// We want physics interaction but no crazy rotation or translation
	mpMeshComponent->BodyInstance.bLockXRotation = true;
	mpMeshComponent->BodyInstance.bLockYRotation = true;
	mpMeshComponent->BodyInstance.LinearDamping = 1;
	mpMeshComponent->BodyInstance.AngularDamping = 1;

	// It'll be used to collect guns
	mpMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShip::OnOverlap);

	mHitPoints = 150;
	mScoreValue = 400;

	mMovingDirection = FVector(0, 1, 0);

	mDirectionChangeCoolDown = 1.5;
	mShootingCoolDown = 1.;
	mImpulseMultiplier = 3.;
	mAimEccentricity = 50.;
}

void AEnemyShip::BeginPlay()
{
	Super::BeginPlay();
	UWorld* pW = GetWorld();

	if (IsValid(pW))
	{
		FTimerHandle th;
		FTimerHandle th2;

		pW->GetTimerManager().SetTimer(th, this, &AEnemyShip::ChangeMovingDirection, mDirectionChangeCoolDown, true);
		// Random initial delay so they don't sync up shooting necessarily
		pW->GetTimerManager().SetTimer(th2, this, &AEnemyShip::Shoot, mShootingCoolDown, true, FMath::FRand()*2.);

		mpGun = pW->SpawnActor<AGun>(AGun::StaticClass(), GetActorTransform());
		mpGun->AttachToPawn(this, FTransform( FRotator(0, 0, 0), FVector(90, 0, 0) ) );
		mpGun->SetGunType(EGunType::SlowGun);
	}
}

void AEnemyShip::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// interpolate rotation aiming moving direction
	FRotator targRot = FRotationMatrix::MakeFromXZ(mMovingDirection, FVector(0, 0, 1)).Rotator();
	FRotator newRot = FMath::RInterpTo(GetActorRotation(), targRot, deltaTime, 5.);

	SetActorRotation(newRot);

	// Move
	mpMeshComponent->AddImpulse(GetActorForwardVector()*mImpulseMultiplier, NAME_None, true);
}

void AEnemyShip::OnOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult & crSweepResult)
{
	AGun* pGun = Cast<AGun>(pOtherActor);

	if (IsValid(pGun))
	{
		if (IsValid(mpGun)) mpGun->Destroy();

		mpGun = pGun;
		pGun->AttachToPawn(this, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}
}


void AEnemyShip::ChangeMovingDirection()
{
	const float cAngle = FMath::Lerp(-90, 90, FMath::FRand());
	mMovingDirection = mMovingDirection.RotateAngleAxis(cAngle, FVector(0, 0, 1));
}

void AEnemyShip::Shoot()
{
	AAsteroidShip* pAsteroidShip = Cast<AAsteroidShip>( UGameplayStatics::GetPlayerPawn(this, 0) );

	if (IsValid(mpGun) && IsValid(pAsteroidShip) && pAsteroidShip->CanTakeDamage())
	{

		FVector shipLoc = pAsteroidShip->GetActorLocation();

		// Aim on player and add eccentricity random vector to maybe miss shot
		FVector targetLoc = shipLoc
			+ FVector(FMath::FRand()-.5, FMath::FRand()-.5, FMath::FRand()-.5).Normalize()
			* FMath::Lerp(-mAimEccentricity, mAimEccentricity, FMath::FRand());

		FTransform t = FTransform();

		// Position gun so it is always between this ship and the target
		t.SetLocation(GetActorLocation() + 90 * (shipLoc - GetActorLocation()).GetSafeNormal());
		t.SetRotation(FRotationMatrix::MakeFromXZ(targetLoc-t.GetLocation() , FVector(0, 0, 1)).ToQuat());

		mpGun->SetActorTransform(t);
		mpGun->Shoot();
	}
}