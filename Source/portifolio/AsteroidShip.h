// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "AsteroidShip.generated.h"

UCLASS()
class PORTIFOLIO_API AAsteroidShip : public APawn, public IDamageInterface
{
	GENERATED_BODY()
		
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mShipMeshComponent;

	/* The audio component */
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* mThrustAudioComponent;

	FVector mCurrentVelocity;
	bool mCanShoot;
	bool mShooting;
	UWorld* mWorld;

	int32 mScore;

public:
	// Sets default values for this pawn's properties
	AAsteroidShip();

	// view target
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	class ACameraActor* mViewTargetCam;

	// max speed in cm/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mMaxSpeed;

	// rotation speed in degrees/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mRotateSpeed;

	// acceleration applied in cm/s²
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mAccel;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mShootPeriod;

	
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mFireSound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;



	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UAsteroidWidget> mHUDClass;

	// Variable to hold the widget After Creating it.
	UAsteroidWidget* mpHUD;


	static const FName mMoveForwardBinding;
	static const FName mRotateRightBinding;
	static const FName mShootBinding;
	static const FName mQuickTurnBinding;


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

	UFUNCTION()
	void QuickTurn();

	void ShootCooldownComplete();
	
	void Shoot();

	void ToggleShooting();

	// Damage Interface


	virtual void ReceiveDeathNotification_Implementation(int32 points) override;

};
