// Fill out your copyright notice in the Description page of Project Settings.

#include "ExtraLife.h"
#include "AsteroidShip.h"


void AExtraLife::ApplyEffect(APawn* pAffectedPawn)
{
	AAsteroidShip* pAst = Cast<AAsteroidShip>(pAffectedPawn);
	if (IsValid(pAst))
	{
		pAst->AddLife();
		Destroy();
	}
}