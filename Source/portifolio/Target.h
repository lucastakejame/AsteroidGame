// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Target.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyDeathSignature, ATarget*, destroyedTargetRef);

UCLASS()
class PORTIFOLIO_API ATarget : public APawn, public IDamageInterface
{
	GENERATED_BODY()
protected:

	UPROPERTY(Category = Assets, VisibleDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* mpMeshComponent;

	// Sound to play each time we fire
	UPROPERTY(Category = Assets, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpSoundExplosion;

	UPROPERTY(Category = TargetSettings, EditAnywhere, BlueprintReadWrite)
	float mHitPoints;

	UPROPERTY(Category = TargetSettings, EditAnywhere, BlueprintReadWrite)
	int32 mScoreValue;

	UPROPERTY(Category = TargetSettings, EditAnywhere, BlueprintReadWrite)
	float mLimitSpeed;


	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	// Sets default values for this actor's properties
	ATarget();

	float GetHitPoints() const { return mHitPoints; }

	void SetHitPoints(float hitPoints) { mHitPoints = hitPoints; }

	int32 GetScoreValue() const { return mScoreValue; }

	void SetScoreValue(int32 scoreValue) { mScoreValue = scoreValue; }

	// Damage Interface
	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;

	// Delegates
	FNotifyDeathSignature mOnDeath;

};
