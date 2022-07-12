// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDGAMES_API AWeapon : public AItem
{
	GENERATED_BODY()
	


public:

	AWeapon();

	virtual void Tick(float DeltaTime) override;


protected:

	
	void StopFalling();



private:

	FTimerHandle ThrowWeaponTimer;

	float ThrowWeaponTime=0.7f;

	bool bFalling=0;


public:


	void ThrowWeapon();


};
