// Fill out your copyright notice in the Description page of Project Settings.

#include "ControlBoost.h"
#include "AsteroidShip.h"

void AControlBoost::ApplyEffect(APawn* pAffectedPawn)
{
	AAsteroidShip* pAst = Cast<AAsteroidShip>(pAffectedPawn);
	if (IsValid(pAst))
	{
		FAsteroidShipStats stats = pAst->GetStats();
		FAsteroidShipStats initStats = pAst->GetInitialStats();

		stats.mLinearAccel = mSpeedBoost*initStats.mLinearAccel;
		stats.mAngularAccel = mRotationalBoost*initStats.mMaxAngularSpeed;

		pAst->SetStats(stats);

		Destroy();
	}
}