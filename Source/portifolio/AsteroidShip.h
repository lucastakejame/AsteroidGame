// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AsteroidShip.generated.h"

class ACameraActor;

UCLASS()
class PORTIFOLIO_API AAsteroidShip : public APawn
{
	GENERATED_BODY()

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mShipMeshComponent;

	FVector mCurrentVelocity;
	bool mCanShoot;
	bool mShooting;
	UWorld* mWorld;

public:
	// Sets default values for this pawn's properties
	AAsteroidShip();

	// view target
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	ACameraActor* mViewTargetCam;

	// max speed in cm/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mMaxSpeed;

	// rotation speed in degrees/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mRotateSpeed;

	// acceleration applied 
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mAccel;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mShootPeriod;
	
	static const FName mMoveForwardBinding;
	static const FName mRotateRightBinding;
	static const FName mShootBinding;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Handle for 
	FTimerHandle mTimerHandle_ShootCooldownComplete;

	void ShootCooldownComplete();
	
	void Shoot();

	void ToggleShooting();
};
