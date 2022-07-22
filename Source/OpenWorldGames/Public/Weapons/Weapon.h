// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Item.h"
#include "OpenWorldGames/Public/AmmoTypes.h"
#include "Weapon.generated.h"




UENUM(BlueprintType)
enum class EWeaponType :uint8 {

	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun")
	, EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle")

	, EWT_MAX UMETA(DisplayName = "DefaultMAX")

};




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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		int32 Ammo=40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		int32 MagazineCapacity = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		EWeaponType WeaponType=EWeaponType::EWT_SubmachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		EAmmoType AmmoType=EAmmoType::EAT_9mm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		FName ReloadMontageSection = FName("ReloadSMG");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		bool bMovingClip = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		FName ClipBoneName=FName("smg_clip");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float Damage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float HeadShotDamage = 20.f;

public:


	void ThrowWeapon();

	FORCEINLINE int32 GetAmmo() const { return Ammo; }

	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }

	void DecrementAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	FORCEINLINE FName GetReloadMontageSection() const { return ReloadMontageSection; }

	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

	void ReloadAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

	bool ClipIsFull();

	FORCEINLINE float GetDamage() const { return Damage; }

	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; }



};
