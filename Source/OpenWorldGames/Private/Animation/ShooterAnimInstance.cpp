// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"







void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime) {

	if (ShooterCharacter==nullptr) {
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter) {

		FVector Velocity = ShooterCharacter->GetVelocity();
		Velocity.Z = 0;
		Speed = Velocity.Size();

		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) {
			bIsAccelerating = 1;

		}
		else {
			bIsAccelerating = 0;
		}


		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		
		if (Speed != 0.f) {
			LastMovementOffsetYaw = MovementOffsetYaw;
		}
		
		bAiming = ShooterCharacter->GetAiming();
		

	}







}

void UShooterAnimInstance::NativeInitializeAnimation() {
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
