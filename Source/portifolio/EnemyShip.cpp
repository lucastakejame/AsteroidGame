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
#include "DebugUtils.h"

AEnemyShip::AEnemyShip() : ATarget()
{
	PrimaryActorTick.bTickEvenWhenPaused = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> enemyMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> enemyMaterialAsset(TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));
	if(enemyMeshAsset.Succeeded()) mMeshComponent->SetStaticMesh(enemyMeshAsset.Object);
	if(enemyMaterialAsset.Succeeded())  mMeshComponent->SetMaterial(0, enemyMaterialAsset.Object);

	mMeshComponent->BodyInstance.bLockXRotation = true;
	mMeshComponent->BodyInstance.bLockYRotation = true;
	mMeshComponent->BodyInstance.LinearDamping = 1;
	mMeshComponent->BodyInstance.AngularDamping = 1;

	// It'll be used to collect guns
	mMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AEnemyShip::OnOverlap);

	mHitPoints = 150;
	mScoreValue = 400;

	mMovingDirection = FVector(0, 1, 0);
}

void AEnemyShip::BeginPlay()
{
	Super::BeginPlay();
	UWorld* w = GetWorld();

	if (IsValid(w))
	{
		FTimerHandle th;
		FTimerHandle th2;

		w->GetTimerManager().SetTimer(th, this, &AEnemyShip::ChangeMovingDirection, 1.5, true);
		w->GetTimerManager().SetTimer(th2, this, &AEnemyShip::Shoot, 1., true);

		mpGun = w->SpawnActor<AGun>(AGun::StaticClass(), GetActorTransform());
		mpGun->AttachToPawn(this, FTransform( FRotator(0, 0, 0), FVector(90, 0, 0) ) );
		mpGun->SetType(EGunType::SlowGun);
	}
}


void AEnemyShip::Destroyed()
{
	Super::Destroyed();
	UWorld* w = GetWorld();

	// Change of spawning gun
	if (IsValid(w) && FMath::FRand() <= .35)
	{
		w->SpawnActor<AGun>(AGun::StaticClass(), GetActorTransform())->SetType((EGunType)(FMath::Rand() % int32(EGunType::EnumSize)));
	}
}

void AEnemyShip::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AGun* gun = Cast<AGun>(OtherActor);

	if (IsValid(gun))
	{
		if (IsValid(mpGun)) mpGun->Destroy();

		mpGun = gun;
		gun->AttachToPawn(this, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}
}


// TODO: Investigate weird movimentation, disappearing from time to time
void AEnemyShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator targRot = FRotationMatrix::MakeFromXZ(mMovingDirection, FVector(0, 0, 1)).Rotator();
	FRotator newRot = FMath::RInterpTo(GetActorRotation(), targRot, DeltaTime, 5.);

	SetActorRotation(newRot);

	mMeshComponent->AddImpulse(GetActorForwardVector()*3., NAME_None, true);
}


void AEnemyShip::ChangeMovingDirection()
{
	const float angle = FMath::Lerp(-90, 90, FMath::FRand());
	mMovingDirection = mMovingDirection.RotateAngleAxis(angle, FVector(0, 0, 1));
}

void AEnemyShip::Shoot()
{
	if (IsValid(mpGun))
	{
		// Position gun so it is always between this ship and the target
		FVector shipLoc = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation();
		FVector targetLoc = shipLoc
			+ FVector(FMath::FRand()-.5, FMath::FRand()-.5, FMath::FRand()-.5).Normalize()
			* FMath::Lerp(-1.f, 1.f, FMath::FRand()) * 50;

		FTransform t = FTransform();

		t.SetLocation(GetActorLocation() + 90 * (shipLoc - GetActorLocation()).GetSafeNormal());
		t.SetRotation(FRotationMatrix::MakeFromXZ(targetLoc-t.GetLocation() , FVector(0, 0, 1)).ToQuat());

		mpGun->SetActorTransform(t);
		mpGun->Shoot();
	}
}