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
#include "Collectable.h"

#include "DebugUtils.h"

const FName AAsteroidShip::mscBindingNameMoveForward("MoveForward");
const FName AAsteroidShip::mscBindingNameRotateRight("MoveRight");
const FName AAsteroidShip::mscBindingNameShoot("Shoot");
const FName AAsteroidShip::mscBindingNameQuickTurn("QuickTurn");
const FName AAsteroidShip::mscBindingNameCursorUp("CursorUp");
const FName AAsteroidShip::mscBindingNameCursorDown("CursorDown");
const FName AAsteroidShip::mscBindingNamePause("Pause");


// Sets default values
AAsteroidShip::AAsteroidShip()
{
 	// Player needs to tick when paused to process input and be able to leave paused state
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;

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

	// this lives in z = 0
	mpMeshComponentShip->SetEnableGravity(false);

	// Basically blocks everything except its own projectiles and itself
	mpMeshComponentShip->SetCollisionProfileName("Player");
	mpMeshComponentShip->OnComponentHit.AddDynamic(this, &AAsteroidShip::OnHit);
	mpMeshComponentShip->OnComponentBeginOverlap.AddDynamic(this, &AAsteroidShip::OnOverlap);
	mpMeshComponentShip->SetNotifyRigidBodyCollision(true);

	mInitialLifeCount = 3;

	Tags.Add(FName("wrappable"));
}

// Called when the game starts or when spawned
void AAsteroidShip::BeginPlay()
{
	Super::BeginPlay();

	ResetShipToBeginState();

	// Changing view target to TopDown camera
	UGameplayStatics::GetPlayerController(this, 0)->SetViewTarget(mpCamViewTarget);

	// Starting with menu opened and game paused
	SetPauseGame(true);

}

void AAsteroidShip::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (!mIsPaused && !mIsDead)
	{
		UpdateShip(deltaTime);
	}
}

// Called to bind functionality to input
void AAsteroidShip::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);

	// axis key bindings
	pPlayerInputComponent->BindAxis(mscBindingNameMoveForward);
	pPlayerInputComponent->BindAxis(mscBindingNameRotateRight);

	// actions key bindings
	pPlayerInputComponent->BindAction(mscBindingNameShoot, EInputEvent::IE_Pressed, this, &AAsteroidShip::EnableShooting).bExecuteWhenPaused = true;
	pPlayerInputComponent->BindAction(mscBindingNameShoot, EInputEvent::IE_Released, this, &AAsteroidShip::DisableShooting).bExecuteWhenPaused = true;

	pPlayerInputComponent->BindAction(mscBindingNameQuickTurn, EInputEvent::IE_Pressed, this, &AAsteroidShip::TurnQuickly).bExecuteWhenPaused = true;
	pPlayerInputComponent->BindAction(mscBindingNameCursorUp, EInputEvent::IE_Pressed, this, &AAsteroidShip::NotifyUpPress).bExecuteWhenPaused = true;
	pPlayerInputComponent->BindAction(mscBindingNameCursorDown, EInputEvent::IE_Pressed, this, &AAsteroidShip::NotifyDownPress).bExecuteWhenPaused = true;

	pPlayerInputComponent->BindAction(mscBindingNamePause, EInputEvent::IE_Pressed, this, &AAsteroidShip::TogglePauseGame).bExecuteWhenPaused = true;

}

void AAsteroidShip::EnableShooting()
{
	mIsShooting = true;

	mOnCursorConfirmation.Broadcast();
}

void AAsteroidShip::DisableShooting()
{
	mIsShooting = false;
}

void AAsteroidShip::NotifyUpPress()
{
	mOnCursorUp.Broadcast();
}

void AAsteroidShip::NotifyDownPress()
{
	mOnCursorDown.Broadcast();
}

void AAsteroidShip::TurnQuickly()
{
	AddActorWorldRotation(FRotator(0, 180, 0));
}

void AAsteroidShip::TogglePauseGame()
{
	mIsPaused = !mIsPaused;
	SetPauseGame(mIsPaused);
}

void AAsteroidShip::SetPauseGame_Implementation(bool isPaused)
{
	mIsPaused = isPaused;
	UGameplayStatics::SetGamePaused(this, isPaused);
}

void AAsteroidShip::OnHit(UPrimitiveComponent* pHitComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, FVector normalImpulse, const FHitResult& crHit)
{
	log("HIT")
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
	log("OVERLAP")
	AGun* pGun = Cast<AGun>(pOtherActor);

	// TODO: Maybe make a interface to deal with this interaction
	ACollectable* pCollec = Cast<ACollectable>(pOtherActor);

	if (IsValid(pGun))
	{
		if (IsValid(mpGun)) mpGun->Destroy();
		mpGun = pGun;
		pGun->AttachToPawn(this, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}
	else if (IsValid(pCollec))
	{
		switch (pCollec->GetType())
		{
		case ECollectableType::ExtraLife:
		{
			AddLife();
		}
		break;
		case ECollectableType::ControlBoost:
		{
			mStats.mLinearAccel = 2.*mInitialStats.mLinearAccel;
			mStats.mAngularAccel = 1.4*mInitialStats.mMaxAngularSpeed;
		}
		break;
		default:
			break;
		}
		pCollec->Destroy();
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

	// Reading player input, in range [-1.f, 1.f]
	const float cForwardValue = GetInputAxisValue(mscBindingNameMoveForward);
	const float cRotatingValue = GetInputAxisValue(mscBindingNameRotateRight);

	// Handle rotation
	mAngularSpeed = (cRotatingValue == 0.) ? 0 :
		FMath::Clamp<float>(mAngularSpeed + cRotatingValue * mStats.mAngularAccel,
			-mStats.mMaxAngularSpeed,
			mStats.mMaxAngularSpeed);

	AddActorWorldRotation(FRotator(0., mAngularSpeed * deltaTime, 0.));

	// Handle translation
	if (cForwardValue > 0.f)
	{
		if (!mpAudioComponentThrust->IsPlaying())
		{
			mpAudioComponentThrust->Play();
		}

		mLinearVelocity = mLinearVelocity + GetActorForwardVector() * mStats.mLinearAccel * deltaTime;
		mLinearVelocity = mLinearVelocity.GetClampedToMaxSize(mStats.mMaxLinearSpeed * deltaTime);
	}
	else
	{
		mpAudioComponentThrust->Stop();
	}

	AddActorWorldOffset(mLinearVelocity);

	// Handle shooting
	if (mIsShooting && !mIsDead)
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

	if (IsValid(mpGun)) mpGun->Destroy();

	mpGun = GetWorld()->SpawnActor<AGun>(AGun::StaticClass());
	if (mpGun)
	{
		mpGun->SetGunType(mInitialStats.mGunType);
		mpGun->AttachToPawn(this, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}

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
		mOnDeath.Broadcast();
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
