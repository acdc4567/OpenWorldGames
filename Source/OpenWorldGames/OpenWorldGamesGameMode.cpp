// Copyright Epic Games, Inc. All Rights Reserved.

#include "OpenWorldGamesGameMode.h"
#include "OpenWorldGamesCharacter.h"
#include "UObject/ConstructorHelpers.h"

AOpenWorldGamesGameMode::AOpenWorldGamesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
