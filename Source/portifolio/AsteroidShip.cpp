// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidShip.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Gun.h"
#include "NormalGun.h"
#include "Collectable.h"

#include "DebugUtils.h"


// Sets default values
AAsteroidShip::AAsteroidShip()
{
 	// Player needs to tick when paused to process input and be able to leave paused state
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> sAssetMeshShip(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundFire(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundThrust(TEXT("/Game/Asteroids/Sounds/SW_ShipPropulsionLoop.SW_ShipPropulsionLoop"));
	static ConstructorHelpers::FObjectFinder<USoundBase> sAssetSoundExplosion(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));

	// Creating components
	mpMeshComponentShip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	mpAudioComponentThrust = CreateDefaultSubobject<UAudioComponent>(TEXT("ThrustAudio"));

	// Component attach hierarchy
	RootComponent = mpMeshComponentShip;
	mpAudioComponentThrust->SetupAttachment(mpMeshComponentShip);

	// Setting defaults
	if(sAssetMeshShip.Succeeded()) mpMeshComponentShip->SetStaticMesh(sAssetMeshShip.Object);
	if(sAssetSoundThrust.Succeeded()) mpAudioComponentThrust->SetSound(sAssetSoundThrust.Object);
	if(sAssetSoundFire.Succeeded()) mpSoundFire = sAssetSoundFire.Object;
	if(sAssetSoundExplosion.Succeeded()) mpSoundExplosion = sAssetSoundExplosion.Object;

	mInitialGunClass = ANormalGun::StaticClass();

	// this lives in z = 0
	mpMeshComponentShip->SetEnableGravity(false);

	// Basically blocks everything except its own projectiles and itself
	mpMeshComponentShip->SetCollisionProfileName("Player");
	mpMeshComponentShip->OnComponentHit.AddDynamic(this, &AAsteroidShip::OnHit);
	mpMeshComponentShip->OnComponentBeginOverlap.AddDynamic(this, &AAsteroidShip::OnOverlap);
	mpMeshComponentShip->SetNotifyRigidBodyCollision(true);

	mInitialLifeCount = 3;

	Tags.Add(FName("wrappable"));

	mpMeshComponentShip->ComponentTags.Add("damageable");
}

// Called when the game starts or when spawned
void AAsteroidShip::BeginPlay()
{
	Super::BeginPlay();

	// Changing view target to TopDown camera
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(mpCamViewTarget);
}

void AAsteroidShip::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!mIsDead)
	{
		UpdateShip(deltaTime);
	}
}

void AAsteroidShip::Destroyed()
{
	if (IsValid(mpGun)) mpGun->Destroy();
}

void AAsteroidShip::EnableShooting()
{
	mIsShooting = true;
}

void AAsteroidShip::DisableShooting()
{
	mIsShooting = false;
}

void AAsteroidShip::TurnQuickly()
{
	AddActorWorldRotation(FRotator(0, 180, 0));
}

void AAsteroidShip::SetGun(AGun* pGun)
{
	if (IsValid(pGun) && pGun != mpGun)
	{
		if (IsValid(mpGun)) mpGun->Destroy();
		mpGun = pGun;
	}
}

void AAsteroidShip::DeleteGun()
{
	if (IsValid(mpGun))
	{
		mpGun->Destroy();
		mpGun = nullptr;
	}
}

void AAsteroidShip::SetForwardValue(float val)
{
	mForwardValue = val;
}

void AAsteroidShip::SetRotatingValue(float val)
{
	mRotatingValue = val;
}

void AAsteroidShip::OnHit(UPrimitiveComponent* pHitComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, FVector normalImpulse, const FHitResult& crHit)
{
	if (pOtherActor
		&& pOtherActor->ActorHasTag(FName("doesDamage"))
		&& !mIsDead) // Without this, its possible to receive a second collision before collision is disabled
	{
		UGameplayStatics::PlaySoundAtLocation(this, mpSoundExplosion, GetActorLocation());
		SubtractLife();
	}
}

void AAsteroidShip::OnOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& crSweepResult)
{
	ACollectable* pCollec = Cast<ACollectable>(pOtherActor);

	if (IsValid(pCollec))
	{
		pCollec->ApplyEffect(this);
	}
}

void AAsteroidShip::UpdateShip(float deltaTime)
{

	// Ghost blinking routine
	if (mIsGhost && GetWorld())
	{
		SetActorHiddenInGame(FMath::Fmod(GetWorld()->GetRealTimeSeconds(), mGhostCooldown / 4.) > mGhostCooldown / 8.);
	}
	else
	{
		SetActorHiddenInGame(false);
	}

	// Handle rotation
	mAngularSpeed = (mRotatingValue == 0.) ? 0 :
		FMath::Clamp<float>(mAngularSpeed + mRotatingValue * mStats.mAngularAccel,
			-mStats.mMaxAngularSpeed,
			mStats.mMaxAngularSpeed);

	AddActorWorldRotation(FRotator(0., mAngularSpeed * deltaTime, 0.));

	// Handle translation
	if (mForwardValue != 0.f)
	{
		if (!mpAudioComponentThrust->IsPlaying())
		{
			mpAudioComponentThrust->Play();
		}

		mLinearVelocity = mLinearVelocity + mForwardValue * GetActorForwardVector() * mStats.mLinearAccel * deltaTime;
		mLinearVelocity = mLinearVelocity.GetClampedToMaxSize(mStats.mMaxLinearSpeed * deltaTime);
	}
	else
	{
		mpAudioComponentThrust->Stop();
	}

	AddActorWorldOffset(mLinearVelocity);

	// Handle shooting
	if (mIsShooting)
	{
		Shoot();
	}

}

// when the game first starts
void AAsteroidShip::ResetShipToBeginState()
{
	ResetShipToReviveState();

	mIsShooting = false;
	mIsGhost = false;
	mIsDead = false;

	mLifeCount = mInitialLifeCount;
	mScore = 0;

	mOnScoreChange.Broadcast(mScore);
	mOnLifeCountChange.Broadcast(mLifeCount);

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AAsteroidShip::ResetShipToReviveState()
{
	SetActorTransform(FTransform(FRotator(0, 0, 0), FVector(0, 0, 0), FVector(1, 1, 1)));

	// reset stats
	mStats = mInitialStats;

	mLinearVelocity = FVector(0, 0, 0);

	// Reset to initial gun
	FActorSpawnParameters sp;
	AGun* pGun = GetWorld()->SpawnActor<AGun>(mInitialGunClass, sp);

	// Attaching gun to this player
	if (IsValid(pGun)) pGun->ApplyEffect(this);
}

void AAsteroidShip::SubtractLife()
{
	mIsDead = true;
	mLifeCount--;

	// Ship can't be affected while dead
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// stop thrust sound
	mpAudioComponentThrust->Stop();

	GetWorld()->GetTimerManager().SetTimer(mTimerHandleDeathCooldown, this, &AAsteroidShip::FinishDeathCooldown, mDeathCooldown);

	mOnLosingLife.Broadcast();
	mOnLifeCountChange.Broadcast(mLifeCount);
}

void AAsteroidShip::AddLife()
{
	mLifeCount++;

	mOnLifeCountChange.Broadcast(mLifeCount);
}

void AAsteroidShip::FinishDeathCooldown()
{
	// If there are lives yet, prepare to revive, otherwise notify real death
	if (mLifeCount > 0)
	{
		if (!mIsGhost)
		{
			mIsDead = false;
			mIsGhost = true;

			ResetShipToReviveState();

			// Set period as ghost
			GetWorld()->GetTimerManager().SetTimer(mTimerHandleDeathCooldown, this, &AAsteroidShip::FinishDeathCooldown, mGhostCooldown);
		}
		else
		{

			// Back to normal
			mIsGhost = false;
			SetActorEnableCollision(true);
		}
	}
	else
	{
		mOnLivesEnded.Broadcast();
	}

}


bool AAsteroidShip::CanTakeDamage()
{
	return !mIsDead && !mIsGhost;
}

void AAsteroidShip::Shoot()
{
	if (mpGun) mpGun->Shoot();
}

void AAsteroidShip::ReceiveDeathNotification_Implementation(int32 points)
{
	mScore += points;

	mOnScoreChange.Broadcast(mScore);
}

void AAsteroidShip::ReceiveDamage_Implementation(APawn* pInstigator, float damage)
{
	if (pInstigator != this)
	{
		UGameplayStatics::PlaySoundAtLocation(this, mpSoundExplosion, GetActorLocation());
		SubtractLife();
	}
}
