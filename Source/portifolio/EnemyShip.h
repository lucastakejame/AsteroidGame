// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "EnemyShip.generated.h"

/**
 *
 */
UCLASS()
class PORTIFOLIO_API AEnemyShip : public ATarget
{
	GENERATED_BODY()

private:
// Members
	FVector mMovingDirection;

	// gun used to shoot player
	class AGun* mpGun = nullptr;

	// time before changing movement direction again
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGun> mInitialGunClass;

	// time before changing movement direction again
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mDirectionChangeCoolDown;

	// cooldown between 1 shot and other, not attached to gun shoot cooldown
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mShootingCoolDown;

	// Will amplify impulse applied in enemy for movement
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mImpulseMultiplier;

	// The bigger this value, higher the chance of aiming poorly towards the player
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mAimEccentricity;



public:
// Methods
	AEnemyShip();

	virtual void BeginPlay() override;

	virtual void Tick(float deltaTime) override;

	virtual void Destroyed() override;

	// Getter & setter

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE AGun* GetGun() const { return mpGun; }

	void SetGun(AGun* pGun);

	// Handle getting collector
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* pOverlappedComponent, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, int32 otherBodyIndex, bool isFromSweep, const FHitResult & crSweepResult);

	// change direction of movement to a random direction within a range
	void ChangeMovingDirection();

	// attack player
	void Shoot();
};
