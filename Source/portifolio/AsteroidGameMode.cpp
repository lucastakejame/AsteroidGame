// Fill out your copyright notice in the Description page of Project Settings.

#include "AsteroidGameMode.h"
#include "AsteroidShip.h"


AAsteroidGameMode::AAsteroidGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AAsteroidShip::StaticClass();
	
}

