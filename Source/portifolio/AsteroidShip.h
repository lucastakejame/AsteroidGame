// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Gun.h"
#include "AsteroidShip.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoParamSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOneIntSignature, int, num);


// Variables reset when reviving related to movement and gun
USTRUCT(BlueprintType)
struct FAsteroidShipStats
{
	GENERATED_BODY()

	// max speed in cm/s
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mMaxLinearSpeed = 1000;

	// max rotation speed in degrees/s
	UPROPERTY(Category = SHOWEY, EditAnywhere, BlueprintReadWrite)
	float mMaxAngularSpeed = 200;

	// acceleration applied in cm/s²
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mLinearAccel = 20;

	// acceleration applied in degrees/s²
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mAngularAccel = 15;

	// gun equiped
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	EGunType mGunType = EGunType::NormalGun;
};

UCLASS(BlueprintType, Blueprintable)
class PORTIFOLIO_API AAsteroidShip : public APawn, public IDamageInterface
{
	GENERATED_BODY()

private:
	// The mesh component
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mpMeshComponentShip;

	// The audio component
	UPROPERTY(Category = Audio, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAudioComponent* mpAudioComponentThrust;

	// sounds that doesn't need to be stopped
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundBase* mpSoundFire;

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundBase* mpSoundExplosion;

	// view target
	UPROPERTY(Category = View, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class ACameraActor* mpCamViewTarget;

	// Initial values for settings
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FAsteroidShipStats mInitialStats;

	// Updating settings
	UPROPERTY()
	FAsteroidShipStats mStats;

	// velocity being added each frame
	UPROPERTY()
	FVector mLinearVelocity;

	// velocity being added each frame
	UPROPERTY()
	float mAngularSpeed;

	// Current gun attached to ship
	UPROPERTY()
	class AGun* mpGun = nullptr;

	UPROPERTY()
	int32 mScore;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 mInitialLifeCount;

	UPROPERTY()
	int32 mLifeCount;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mDeathCooldown = 1.5;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mGhostCooldown = 1.5;

	// While isGhost, this ship won't take hits or collide with collectables
	UPROPERTY()
	bool mIsGhost;

	// When on, Ship won't appear in the screen
	UPROPERTY()
	bool mIsDead;

	// is player holding shoot button
	UPROPERTY()
	bool mIsShooting;

	// is game paused
	UPROPERTY()
	bool mIsPaused;

	// timer to count death and ghost time
	FTimerHandle mTimerHandleDeathCooldown;

	// input binding names
	static const FName mscBindingNameMoveForward;
	static const FName mscBindingNameRotateRight;
	static const FName mscBindingNameShoot;
	static const FName mscBindingNameQuickTurn;
	static const FName mscBindingNameCursorUp;
	static const FName mscBindingNameCursorDown;
	static const FName mscBindingNamePause;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Delegates start with On and follows the action that triggered it
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FNoParamSignature mOnCursorUp;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FNoParamSignature mOnCursorDown;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FNoParamSignature mOnCursorConfirmation;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FNoParamSignature mOnDeath;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FOneIntSignature mOnScoreChange;

	UPROPERTY(BlueprintAssignable, Category = "Event Dispatcher")
	FOneIntSignature mOnLifeCountChange;

// Methods

	// Sets default values for this pawn's properties
	AAsteroidShip();

	// Getters
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool GetIsDead() const { return mIsDead; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool GetIsPaused() const { return mIsPaused; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool GetIsGhost() const { return mIsGhost; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool GetIsShooting() const { return mIsShooting; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FAsteroidShipStats GetStats() const { return mStats; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FAsteroidShipStats GetInitialStats() const { return mInitialStats; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE AGun* GetGun() const { return mpGun; }

	// Called every frame
	virtual void Tick(float deltaTime) override;

	// input binding
	virtual void SetupPlayerInputComponent(class UInputComponent* pPlayerInputComponent) override;

	// input triggered methods
	void EnableShooting();
	void DisableShooting();
	void NotifyUpPress();
	void NotifyDownPress();

	UFUNCTION()
	void TurnQuickly();

	UFUNCTION(BlueprintCallable)
	void TogglePauseGame();


	// Setters
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetPauseGame(bool isPaused);
	void SetPauseGame_Implementation(bool isPaused);

	void SetStats(FAsteroidShipStats stats) { mStats = stats; }
	
	void SetGun(AGun* pGun)
	{
		if (IsValid(pGun))
		{
			if (IsValid(mpGun)) mpGun->Destroy();
			mpGun = pGun;
		}
	}


	// Handling encounters
	// These HAD to be called OnHit and OnOverlap otherwise unreal wouldn't callback
	UFUNCTION()
	void OnHit(UPrimitiveComponent* pHitComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, FVector normalImpulse, const FHitResult& crHit);

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult& crSweepResult);

	// Handle input to update ship movement, shoot and blink
	UFUNCTION(BlueprintCallable)
	void UpdateShip(float deltaTime);

	// Setup ship for the game beginning
	UFUNCTION(BlueprintCallable)
	void ResetShipToBeginState();

	// Setup position, gun and stats to initial conditions.
	UFUNCTION(BlueprintCallable)
	void ResetShipToReviveState();

	UFUNCTION(BlueprintCallable)
	void SubtractLife();

	UFUNCTION(BlueprintCallable)
	void AddLife();

	// routine for when death and ghost cooldown finish
	void FinishDeathCooldown();

	// Returns if player is ready to take damage
	bool CanTakeDamage();

	// spawn projectile
	void Shoot();

	// Damage Interface
	virtual void ReceiveDeathNotification_Implementation(int32 points) override;

	virtual void ReceiveDamage_Implementation(APawn* pInstigator, float damage) override;

};
