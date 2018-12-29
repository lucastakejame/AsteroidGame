// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "AsteroidShip.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneIntSignature, int, num);


UCLASS(BlueprintType, Blueprintable)
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
	bool mIsGhost;
	bool mIsDead;
	UWorld* mWorld;

	int32 mScore;
	int32 mLifeCount;

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

// members

	// view target
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	class ACameraActor* mpViewTargetCam;

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

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mDeathCooldown;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool mIsPaused;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpFireSound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpExplosionSound;

	static const FName mMoveForwardBinding;
	static const FName mRotateRightBinding;
	static const FName mShootBinding;
	static const FName mQuickTurnBinding;
	static const FName mCursorUp;
	static const FName mCursorDown;
	static const FName mPause;

	// timers
	FTimerHandle mTimerHandle_ShootCooldown;
	FTimerHandle mTimerHandle_DeathCooldown;


	// Delegates

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FNoParamSignature mCursorUpDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FNoParamSignature mCursorDownDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FNoParamSignature mCursorConfirmDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FNoParamSignature mNotifyDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FOneIntSignature mScoreUpdateDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
		FOneIntSignature mLifeCountUpdateDelegate;

// Methods

	// Sets default values for this pawn's properties
	AAsteroidShip();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void QuickTurn();

	UFUNCTION(BlueprintCallable)
	void UpdateShip(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void ResetShipState();

	void ShootCooldownComplete();

	void SubtractLife();

	void DeathCooldownComplete();
	
	// input triggered methods
	void Shoot();
	void EnableShooting();
	void DisableShooting();
	void NotifyUpPress();
	void NotifyDownPress();

	UFUNCTION(BlueprintCallable)
	void TogglePauseGame();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPauseGame(bool isPaused);
	void SetPauseGame_Implementation(bool isPaused);
	
	// Damage Interface
	virtual void ReceiveDeathNotification_Implementation(int32 points) override;

};
