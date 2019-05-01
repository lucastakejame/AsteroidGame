// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenLimitManager.generated.h"

UCLASS(BlueprintType)
class PORTIFOLIO_API AScreenLimitManager : public AActor
{
	GENERATED_BODY()
private:

	UPROPERTY(EditAnywhere)
	float mLimitWidth;

	UPROPERTY(EditAnywhere)
	float mLimitHeight;

	UPROPERTY(EditAnywhere)
	bool mShouldDrawDebugLimits;

public:

	// Sets default values for this actor's properties
	AScreenLimitManager();

	virtual void OnConstruction(const FTransform& crT) override;

	// Keeping actors in limits
	virtual void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetLimitWidth() const { return mLimitWidth; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE float GetLimitHeight() const { return mLimitHeight; }

	// corner0 has (minX,minY), corner1 has (maxX, maxY)
	UFUNCTION(BlueprintCallable)
	FVector WrapLocation(FVector location, FVector corner0, FVector corner1);

	// Returns a random spot inside limits
	UFUNCTION(BlueprintCallable)
	FVector GetRandomLocationInsideLimits() const;

};
