// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ShooterCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShooterCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLDGAMES_API IShooterCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		 void IncrementsOverlappedItemCount(int32 Amount);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void GetsPickupItem(class AItem* Item);



};
