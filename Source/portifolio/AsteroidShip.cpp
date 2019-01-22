// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidShip.h"
#include "Projectile.h"
#include "TimerManager.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Gun.h"

#include "AsteroidWidget.h"
#include "Blueprint/UserWidget.h"

#include "DebugUtils.h"

const FName AAsteroidShip::mMoveForwardBinding("MoveForward");
const FName AAsteroidShip::mRotateRightBinding("MoveRight");
const FName AAsteroidShip::mShootBinding("Shoot");
const FName AAsteroidShip::mQuickTurnBinding("QuickTurn");
const FName AAsteroidShip::mCursorUp("CursorUp");
const FName AAsteroidShip::mCursorDown("CursorDown");
const FName AAsteroidShip::mPause("Pause");


// Sets default values
AAsteroidShip::AAsteroidShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> shipMeshAsset(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));
	static ConstructorHelpers::FObjectFinder<USoundBase> fireSoundAsset(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<USoundBase> thrustSoundAsset(TEXT("/Game/Asteroids/Sounds/SW_ShipPropulsionLoop.SW_ShipPropulsionLoop"));
	static ConstructorHelpers::FObjectFinder<USoundBase> explosionSoundAsset(TEXT("/Game/Asteroids/Sounds/SW_AsteroidExplosion.SW_AsteroidExplosion"));

	mShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = mShipMeshComponent;
	mThrustAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ThrustAudio"));
	mThrustAudioComponent->SetupAttachment(mShipMeshComponent);

	mShipMeshComponent->SetEnableGravity(false);
	mShipMeshComponent->SetStaticMesh(shipMeshAsset.Object);
	mShipMeshComponent->SetCollisionProfileName("Player");
	mShipMeshComponent->OnComponentHit.AddDynamic(this, &AAsteroidShip::OnHit);
	mShipMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AAsteroidShip::OnOverlap);
	mShipMeshComponent->SetNotifyRigidBodyCollision(true);

	mpFireSound = fireSoundAsset.Object;
	mpExplosionSound = explosionSoundAsset.Object;

	mThrustAudioComponent->SetSound(thrustSoundAsset.Object);

	Tags.Add(FName("wrappable"));

}

void AAsteroidShip::ResetShipState()
{
	FTransform t = FTransform(FRotator(0,0,0), FVector(0, 0, 0), FVector(1, 1, 1));

	SetActorTransform(t);

	// Default Values
	mMaxSpeed = mInitialInfo.mMaxSpeed;
	mRotateSpeed = mInitialInfo.mRotateSpeed;
	mAccel = mInitialInfo.mAccel;
	mDeathCooldown = mInitialInfo.mDeathCooldown;

	mLifeCount = 3;
	mScore = 0;

	mCurrentVelocity = FVector(0, 0, 0);
	mShooting = false;

	mIsGhost = false;
	mIsDead = false;

	mScoreUpdateDelegate.Broadcast(mScore);
	mLifeCountUpdateDelegate.Broadcast(mLifeCount);

	ResetGun();

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AAsteroidShip::ResetGun()
{
	if (IsValid(mWorld))
	{
		if (IsValid(mpGun)) mpGun->Destroy();

		mpGun = mWorld->SpawnActor<AGun>(AGun::StaticClass());
		mpGun->SetGunType(mInitialInfo.mGunType);
		mpGun->AttachToPawn(this, FTransform(FRotator(0, 0, 0), FVector(90, 0, 0)));
	}
}

// Called when the game starts or when spawned
void AAsteroidShip::BeginPlay()
{
	Super::BeginPlay();

	mWorld = GetWorld();

	ResetShipState();

	SetPauseGame(true);

	mThrustAudioComponent->Stop();
}

void AAsteroidShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAsteroidShip::UpdateShip(float DeltaTime)
{

	if (mIsDead)
	{
		mThrustAudioComponent->Stop();
		return;
	}


	const float accelPerFrame = mAccel*DeltaTime;

	const float forwardValue = GetInputAxisValue(mMoveForwardBinding);
	const float rotatingValue = GetInputAxisValue(mRotateRightBinding);

	this->AddActorWorldRotation(FRotator(0., rotatingValue*mRotateSpeed*DeltaTime, 0.));

	if (mIsGhost && mWorld)
	{
		SetActorHiddenInGame(FMath::Fmod(mWorld->GetRealTimeSeconds(), mDeathCooldown / 4.) > mDeathCooldown / 8.);
	}
	else
	{
		SetActorHiddenInGame(false);
	}

	// move pressed
	if (forwardValue > 0.f)
	{
		if (!mThrustAudioComponent->IsPlaying())
		{
			mThrustAudioComponent->Play();
		}

		mCurrentVelocity = mCurrentVelocity + GetActorForwardVector() * accelPerFrame;
		mCurrentVelocity = mCurrentVelocity.GetClampedToMaxSize(mMaxSpeed * DeltaTime);
	}
	else
	{
		mThrustAudioComponent->Stop();
	}

	this->AddActorWorldOffset(mCurrentVelocity);

	if (mShooting && !mIsDead)
	{
		Shoot();
	}

}

void AAsteroidShip::Shoot()
{
	if (mpGun) mpGun->Shoot();
}

void AAsteroidShip::EnableShooting()
{
	mShooting = true;

	mCursorConfirmDelegate.Broadcast();
}

void AAsteroidShip::DisableShooting()
{
	mShooting = false;
}

void AAsteroidShip::QuickTurn()
{
	AddActorWorldRotation(FRotator(0, 180, 0));
}
// Called to bind functionality to input
void AAsteroidShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// axis key bindings
	PlayerInputComponent->BindAxis(mMoveForwardBinding);
	PlayerInputComponent->BindAxis(mRotateRightBinding);

	// actions key bindings
	PlayerInputComponent->BindAction(mShootBinding, EInputEvent::IE_Pressed, this, &AAsteroidShip::EnableShooting).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction(mShootBinding, EInputEvent::IE_Released, this, &AAsteroidShip::DisableShooting).bExecuteWhenPaused = true;

	PlayerInputComponent->BindAction(mQuickTurnBinding, EInputEvent::IE_Pressed, this, &AAsteroidShip::QuickTurn).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction(mCursorUp, EInputEvent::IE_Pressed, this, &AAsteroidShip::NotifyUpPress).bExecuteWhenPaused = true;
	PlayerInputComponent->BindAction(mCursorDown, EInputEvent::IE_Pressed, this, &AAsteroidShip::NotifyDownPress).bExecuteWhenPaused = true;

	PlayerInputComponent->BindAction(mPause, EInputEvent::IE_Pressed, this, &AAsteroidShip::TogglePauseGame).bExecuteWhenPaused = true;

}

void AAsteroidShip::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor
		&& OtherActor->ActorHasTag(FName("doesDamage"))
		&& !mIsDead) // Without this, its possible to receive a second collision before collision is disabled
	{
		UGameplayStatics::PlaySoundAtLocation(this, mpExplosionSound, GetActorLocation());
		SubtractLife();
	}
}

void AAsteroidShip::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AGun* gun = Cast<AGun>(OtherActor);

	if (IsValid(gun))
	{
		if (IsValid(mpGun)) mpGun->Destroy();
		mpGun = gun;
		gun->AttachToPawn(this, FTransform(FRotator(0,0,0), FVector(90, 0, 0) ) );
	}
}


void AAsteroidShip::SubtractLife()
{
	mIsDead = true;
	mLifeCount--;
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	ResetGun();

	mWorld->GetTimerManager().SetTimer(mTimerHandle_DeathCooldown, this, &AAsteroidShip::DeathCooldownComplete, mDeathCooldown);

	mLifeCountUpdateDelegate.Broadcast(mLifeCount);
}


void AAsteroidShip::DeathCooldownComplete()
{
	if (mLifeCount > 0)
	{
		if (!mIsGhost)
		{
			mIsDead = false;
			mIsGhost = true;

			SetActorLocationAndRotation(FVector(0, 0, 0), FRotator(0, 0, 0));
			mCurrentVelocity = FVector(0, 0, 0);

			mWorld->GetTimerManager().SetTimer(mTimerHandle_DeathCooldown, this, &AAsteroidShip::DeathCooldownComplete, mDeathCooldown / 2.);
		}
		else
		{
			mIsGhost = false;
			SetActorEnableCollision(true);
		}
	}
	else
	{
		mNotifyDeathDelegate.Broadcast();
	}

}

void AAsteroidShip::ReceiveDeathNotification_Implementation(int32 points)
{
	mScore += points;

	mScoreUpdateDelegate.Broadcast(mScore);
}


void AAsteroidShip::ReceiveDamage_Implementation(APawn* instigator, float damage)
{
	if (instigator != this)
	{
		UGameplayStatics::PlaySoundAtLocation(this, mpExplosionSound, GetActorLocation());
		SubtractLife();
	}
}

void AAsteroidShip::NotifyUpPress()
{
	mCursorUpDelegate.Broadcast();
}
void AAsteroidShip::NotifyDownPress()
{
	mCursorDownDelegate.Broadcast();
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