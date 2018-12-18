// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "portifolioGameMode.h"
#include "portifolioPawn.h"
#include "GameFramework/Actor.h"

AportifolioGameMode::AportifolioGameMode()
{
	// set default pawn class to our character class
	DefaultPawnClass = AportifolioPawn::StaticClass();
}