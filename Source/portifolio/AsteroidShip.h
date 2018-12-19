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

	/* The mesh component */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* mThrustAudioComponent;

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
	
	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mFireSound;

	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;

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


	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ShootCooldownComplete();
	
	void Shoot();

	void ToggleShooting();
};
