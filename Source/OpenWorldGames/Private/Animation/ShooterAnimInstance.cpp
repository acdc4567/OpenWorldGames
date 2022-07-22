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
		
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		
		if (bReloading) {
			OffsetState = EOffsetState::EOS_Reloading;

		}
		else if (bIsInAir) {
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming()) {
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else {
			OffsetState = EOffsetState::EOS_Hip;
		}


		bAiming = ShooterCharacter->GetAiming();
		TurnInPlace();
		Lean(DeltaTime);
	}
	






}

void UShooterAnimInstance::NativeInitializeAnimation() {
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace() {

	if (ShooterCharacter == nullptr)return;
	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;



	if (Speed > 0.f) {

		RootYawOffset = 0.f;
		TIPCharacterYaw= ShooterCharacter->GetActorRotation().Yaw;
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		RotationCurveLastFrame = 0.f;
		RotationCurve = 0.f;

	}
	else {
		TIPCharacterYawLastFrame = TIPCharacterYaw;
		TIPCharacterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta = TIPCharacterYaw - TIPCharacterYawLastFrame;

		RootYawOffset -= TIPYawDelta;


		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		const float Turning{ GetCurveValue(TEXT("Turnin")) };
		if (Turning > 0.f) {
			RotationCurveLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
		
			const float DeltaRotation{ RotationCurve - RotationCurveLastFrame };
		
			if (RootYawOffset > 0.f) {
				RootYawOffset -= DeltaRotation;
			}
			else {
				RootYawOffset += DeltaRotation;
			}

			const float ABSRootYawOffset = FMath::Abs(RootYawOffset);
			if (ABSRootYawOffset > 90.f) {
				const float YawExcess = ABSRootYawOffset - 90.f;
				RootYawOffset > 0.f ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}

		}

		/*
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("TIPCharacterYaw : %f"), TIPCharacterYaw));
			GEngine->AddOnScreenDebugMessage(2, -1, FColor::Cyan, FString::Printf(TEXT("RootYawOff : %f"), RootYawOffset));

		}
		*/

	}





}

void UShooterAnimInstance::Lean(float DeltaTime) {
	if (ShooterCharacter == nullptr)return;
	CharacterRotLastFrame = CharacterRotation;
	CharacterRotation = ShooterCharacter->GetActorRotation();
	
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotLastFrame, CharacterRotation);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f);
	YawDelta = FMath::Clamp(Interp, -90.f, 90.f);
	
	
	/*
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, -1, FColor::Cyan, FString::Printf(TEXT("YawDelta : %f"), YawDelta));
		
	}*/





}
