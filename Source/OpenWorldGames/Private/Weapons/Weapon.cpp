// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Weapon.h"

AWeapon::AWeapon() {

	PrimaryActorTick.bCanEverTick = 1;


}

void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);


	if (GetItemState() == EItemState::EIS_Falling && bFalling) {
		const FRotator MeshRotation{ 0.f,GetItemMesh()->GetComponentRotation().Yaw,0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, 0, nullptr, ETeleportType::TeleportPhysics);
	}

}

void AWeapon::ThrowWeapon() {

	FRotator MeshRotation{ 0.f,GetItemMesh()->GetComponentRotation().Yaw,0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, 0, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	FVector MeshRight = GetItemMesh()->GetRightVector();
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	float RandomRotation = 30.f;
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection *= 50.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = 1;
	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);

}

void AWeapon::StopFalling() {

	bFalling = 0;
	SetItemState(EItemState::EIS_Pickup);



}
