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
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "Camera/CameraActor.h"

#include "DebugUtils.h"

const FName AAsteroidShip::mMoveForwardBinding("MoveForward");
const FName AAsteroidShip::mRotateRightBinding("MoveRight");
const FName AAsteroidShip::mShootBinding("Shoot");

// Sets default values
AAsteroidShip::AAsteroidShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Mesh setup
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));

	mShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	mShipMeshComponent->SetEnableGravity(false);
	mShipMeshComponent->SetStaticMesh(ShipMesh.Object);

	RootComponent = mShipMeshComponent;

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
		mCurrentVelocity = mCurrentVelocity + GetActorForwardVector() * accelPerFrame;
		mCurrentVelocity = mCurrentVelocity.GetClampedToMaxSize(mMaxSpeed * DeltaTime);
	}
	
	this->AddActorWorldOffset(mCurrentVelocity);

	if (mShooting)
	{
		log("SHOOT")
		Shoot();
	}

		
}

void AAsteroidShip::Shoot()
{
	if (!mWorld)
	{
		return;
	}

	log("CAN SHOOT?");

	if (!mCanShoot)
	{
		return;
	}
	log("YES!");


	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	params.bNoFail = true;

	AportifolioProjectile* spawned = mWorld->SpawnActor<AportifolioProjectile>(
		this->GetActorLocation()
		+ this->GetActorForwardVector()*90.f,
		this->GetActorRotation(), params
	);

	mCanShoot = false;

	mWorld->GetTimerManager().SetTimer(mTimerHandle_ShootCooldownComplete, this, &AAsteroidShip::ShootCooldownComplete, mShootPeriod);
		
}

void AAsteroidShip::ShootCooldownComplete()
{
	mCanShoot = true;
}

void AAsteroidShip::ToggleShooting()
{
	log("TOGGLING");
	mShooting ^= 1;
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
}