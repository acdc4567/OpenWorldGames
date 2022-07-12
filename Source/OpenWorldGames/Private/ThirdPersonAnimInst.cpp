// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonAnimInst.h"
#include "OpenWorldGames/OpenWorldGamesCharacter.h"




UThirdPersonAnimInst::UThirdPersonAnimInst() {

}

void UThirdPersonAnimInst::UpdateAnimationProperties(float DeltaTime) {
	if (ShooterCharacter == nullptr) {
		ShooterCharacter = Cast<AOpenWorldGamesCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter) {
		FVector Velocity{ ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();
	}
}

void UThirdPersonAnimInst::NativeInitializeAnimation() {
	
	ShooterCharacter = Cast<AOpenWorldGamesCharacter>(TryGetPawnOwner());

}

void UThirdPersonAnimInst::UeLog_PrintOutputLog_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("InterfaceCall"));


}
