// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectable.generated.h"

UENUM()
enum class ECollectableType : uint8
{
	ExtraLife,
	ControlBoost,
	Gun,
	EnumSize
};


UCLASS()
class PORTIFOLIO_API ACollectable : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Assets, VisibleDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* mpMeshComponent;

	UPROPERTY(Category = Assets, VisibleDefaultsOnly, BlueprintReadWrite)
	class UMaterialInstanceDynamic* mpMID;

	ECollectableType mType;

public:
	// Sets default values for this actor's properties
	ACollectable();

	virtual void OnConstruction(const FTransform& crT) override;

	// Getter and Setter
	ECollectableType GetType() { return mType; }

	// Also sets material parameters
	UFUNCTION()
	void SetCollectableType(ECollectableType type);
};
