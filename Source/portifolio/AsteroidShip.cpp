// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidShip.h"
#include "portifolioProjectile.h"
#include "portifolioPawn.h"
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

#include "AsteroidWidget.h"
#include "Blueprint/UserWidget.h"

#include "DebugUtils.h"

const FName AAsteroidShip::mMoveForwardBinding("MoveForward");
const FName AAsteroidShip::mRotateRightBinding("MoveRight");
const FName AAsteroidShip::mShootBinding("Shoot");
const FName AAsteroidShip::mQuickTurnBinding("QuickTurn");


// Sets default values
AAsteroidShip::AAsteroidShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	mShipMeshComponent->SetCollisionProfileName("BlockAllDynamic");
	mShipMeshComponent->OnComponentHit.AddDynamic(this, &AAsteroidShip::OnHit);
	mShipMeshComponent->SetNotifyRigidBodyCollision(true);

	mFireSound = fireSoundAsset.Object;
	mExplosionSound = explosionSoundAsset.Object;

	mThrustAudioComponent->SetSound(thrustSoundAsset.Object);

	// Default Values
	mMaxSpeed = 1000;
	mRotateSpeed = 200;
	mAccel = 10;
	mShootPeriod = .1;
	mCurrentVelocity = FVector(0, 0, 0);
	mCanShoot = true;
	mShooting = false;
}

// Called when the game starts or when spawned
void AAsteroidShip::BeginPlay()
{
	Super::BeginPlay();

	mWorld = GetWorld();


	// Adding HUD to viewport
	if (mHUDClass)
	{
		mpHUD = CreateWidget<UAsteroidWidget>(UGameplayStatics::GetPlayerController(this, 0), mHUDClass, FName("W_HUD"));

		if(mpHUD)
		{
			mpHUD->AddToViewport();
		}
	}
	else
	{
		log("Missing widget HUD class reference.");
	}

}

// Called every frame
void AAsteroidShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	const float accelPerFrame = mAccel*DeltaTime;

	const float forwardValue = GetInputAxisValue(mMoveForwardBinding);
	const float rotatingValue = GetInputAxisValue(mRotateRightBinding);

	this->AddActorWorldRotation(FRotator(0., rotatingValue*mRotateSpeed*DeltaTime, 0.));

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

	if (mShooting)
	{
		Shoot();
	}

}

void AAsteroidShip::Shoot()
{
	if (!mWorld)
	{
		return;
	}

	if (!mCanShoot)
	{
		return;
	}

	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//params.bNoFail = true;
	params.Instigator = this;

	AportifolioProjectile* projectile = mWorld->SpawnActor<AportifolioProjectile>(
		this->GetActorLocation()
		+ this->GetActorForwardVector()*90.f,
		this->GetActorRotation(), params
	);

	projectile->SetDamage(50);


	// Recoil
	// mCurrentVelocity = mCurrentVelocity - GetActorForwardVector()/5;

	if (mFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, mFireSound, GetActorLocation());
	}

	mCanShoot = false;

	mWorld->GetTimerManager().SetTimer(mTimerHandle_ShootCooldownComplete, this, &AAsteroidShip::ShootCooldownComplete, mShootPeriod);

}

void AAsteroidShip::ShootCooldownComplete()
{
	mCanShoot = true;
}

void AAsteroidShip::ToggleShooting()
{
	mShooting ^= 1;
}

void AAsteroidShip::QuickTurn()
{
	AddActorWorldRotation(FRotator(0, 180, 0));
}
// Called to bind functionality to input
void AAsteroidShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// set up key bindings
	PlayerInputComponent->BindAxis(mMoveForwardBinding);
	PlayerInputComponent->BindAxis(mRotateRightBinding);
	PlayerInputComponent->BindAction(mShootBinding, EInputEvent::IE_Pressed, this, &AAsteroidShip::ToggleShooting);
	PlayerInputComponent->BindAction(mShootBinding, EInputEvent::IE_Released, this, &AAsteroidShip::ToggleShooting);
	PlayerInputComponent->BindAction(mQuickTurnBinding, EInputEvent::IE_Pressed, this, &AAsteroidShip::QuickTurn);

}

void AAsteroidShip::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("doesDamage")))
	{
		UGameplayStatics::PlaySoundAtLocation(this, mExplosionSound, GetActorLocation());
		Destroy();
	}
}

void AAsteroidShip::ReceiveDeathNotification_Implementation(int32 points)
{
	mScore += points;

	mpHUD->UpdateScore(mScore);
}