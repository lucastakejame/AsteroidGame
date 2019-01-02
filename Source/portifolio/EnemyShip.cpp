// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyShip.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "PortifolioProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DebugUtils.h"

AEnemyShip::AEnemyShip() : ATarget()
{
	PrimaryActorTick.bTickEvenWhenPaused = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> enemyMeshAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UMaterial> enemyMaterialAsset(TEXT("/Engine/MapTemplates/Materials/BasicAsset01.BasicAsset01"));

	mMeshComponent->SetStaticMesh(enemyMeshAsset.Object);

	mMeshComponent->BodyInstance.bLockXRotation = true;
	mMeshComponent->BodyInstance.bLockYRotation = true;
	mMeshComponent->BodyInstance.LinearDamping = 1;
	mMeshComponent->BodyInstance.AngularDamping = 1;


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
		FTimerHandle disposable;
		FTimerHandle disposable2;

		w->GetTimerManager().SetTimer(disposable, this, &AEnemyShip::ChangeMovingDirection, 1.5, true);
		w->GetTimerManager().SetTimer(disposable2, this, &AEnemyShip::Shoot, 1., true);
	}
}

void AEnemyShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator targRot = FRotationMatrix::MakeFromXZ(mMovingDirection, FVector(0, 0, 1)).Rotator();
	FRotator newRot = FMath::RInterpTo(GetActorRotation(), targRot, DeltaTime, 5.);
	
	SetActorRotation(newRot);

	mMeshComponent->AddImpulse(GetActorForwardVector()*2., NAME_None, true);
}


void AEnemyShip::ChangeMovingDirection()
{
	const float angle = FMath::Lerp(-90, 90, FMath::FRand());
	mMovingDirection = mMovingDirection.RotateAngleAxis(angle, FVector(0, 0, 1));
}

void AEnemyShip::Shoot()
{

	UWorld* w = GetWorld();

	if (IsValid(w))
	{
		FVector targetLoc = UGameplayStatics::GetPlayerPawn(this, 0)->GetActorLocation()
			+ FVector(FMath::FRand(), FMath::FRand(), FMath::FRand()).Normalize()
			* FMath::Lerp(-1.f, 1.f, FMath::FRand()) * 50
			;

		FTransform t = FTransform();

		t.SetLocation(GetActorLocation() + 90 * GetActorForwardVector());
		t.SetRotation(FRotationMatrix::MakeFromXZ(targetLoc-t.GetLocation() , FVector(0, 0, 1)).ToQuat());

		FActorSpawnParameters sp = FActorSpawnParameters();
		sp.Instigator = this;
		sp.Owner = this;

		AportifolioProjectile* proj = w->SpawnActor<AportifolioProjectile>(AportifolioProjectile::StaticClass(), t, sp);

		proj->SetLifeSpan(1.);
		proj->SetDamage(50.);
		proj->GetProjectileMovement()->MaxSpeed = 1500;
	}

}