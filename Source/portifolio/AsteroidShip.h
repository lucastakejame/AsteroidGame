// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "AsteroidShip.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneIntSignature, int, num);

USTRUCT(BlueprintType)
struct FAsteroidShipInfo
{
	GENERATED_BODY()
	
	// max speed in cm/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float mMaxSpeed = 1000;

	// rotation speed in degrees/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float mRotateSpeed = 200;

	// acceleration applied in cm/s²
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float mAccel = 10;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float mDeathCooldown = 1.5;
};

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
	bool mShooting;
	UWorld* mWorld;

	class AGun* mpGun = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Category = ShipState, EditAnywhere, BlueprintReadWrite)
	int32 mScore;

	UPROPERTY(Category = ShipState, EditAnywhere, BlueprintReadWrite)
	int32 mLifeCount;

	UPROPERTY(Category = ShipState, EditAnywhere, BlueprintReadWrite)
	bool mIsGhost;

	UPROPERTY(Category = ShipState, EditAnywhere, BlueprintReadWrite)
	bool mIsDead;

public:

// members

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	FAsteroidShipInfo mInitialInfo;

	// view target
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	class ACameraActor* mpViewTargetCam;

	// max speed in cm/s
	float mMaxSpeed;

	// rotation speed in degrees/s
	float mRotateSpeed;

	// acceleration applied in cm/s²
	float mAccel;


	float mDeathCooldown;

	UPROPERTY(Category = Gameplay, BlueprintReadOnly)
	bool mIsPaused;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpFireSound;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpExplosionSound;

	// input binding names
	static const FName mMoveForwardBinding;
	static const FName mRotateRightBinding;
	static const FName mShootBinding;
	static const FName mQuickTurnBinding;
	static const FName mCursorUp;
	static const FName mCursorDown;
	static const FName mPause;

	// timers
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
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void QuickTurn();

	UFUNCTION(BlueprintCallable)
	void UpdateShip(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void ResetShipState();

	UFUNCTION(BlueprintCallable)
	void ResetGun();
	
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

	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;

};
