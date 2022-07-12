// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Interfaces/ItemInterface.h"
#include "Weapons/Weapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapons/Item.h"




// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 180.f;
	CameraBoom->bUsePawnControlRotation = 1;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = 0;

	bUseControllerRotationPitch = 0;
	bUseControllerRotationYaw = 1;
	bUseControllerRotationRoll = 0;

	GetCharacterMovement()->bOrientRotationToMovement = 0;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;



}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (FollowCamera) {
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}


	EquipWeapon(SpawnDefaultWeapon());




}

void AShooterCharacter::MoveForward(float Value) {

	if (Controller!=nullptr&&Value!=0.f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

	}

}

void AShooterCharacter::MoveRight(float Value) {

	if (Controller != nullptr && Value != 0.f) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation{ 0.f,Rotation.Yaw,0.f };

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

	}


}

void AShooterCharacter::TurnAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());

}

void AShooterCharacter::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());



}

void AShooterCharacter::Turn(float Rate) {
	if (bAiming) {
		AddControllerYawInput(Rate * MouseAimTurnRate);

	}
	else {
		AddControllerYawInput(Rate * MouseHipTurnRate);

	}
	
}

void AShooterCharacter::LookUp(float Rate) {
	if (bAiming) {
		AddControllerPitchInput(Rate * MouseAimLookUpRate);

	}
	else {
		AddControllerPitchInput(Rate * MouseHipLookUpRate);

	}
	
}

void AShooterCharacter::FireWeapon() {
	if (FireSound) {
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket) {
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
	
		if (MuzzleFlash) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);;
		}

		
		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd) {
			if (ImpactParticles) {

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEnd);

			}
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
			if (Beam) {
				Beam->SetVectorParameter(FName("Target"), BeamEnd);

			}
		}



		
		
	
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HipFireMontage) {
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}


	StartCrosshairBulletFire();


}

bool AShooterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation) {
	
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult,OutBeamLocation);
	
	if (bCrosshairHit) {
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else {

	}

	//Second Trace

	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation;
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd * 1.25f;
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);

	if (WeaponTraceHit.bBlockingHit) {
		OutBeamLocation = WeaponTraceHit.Location;
		return 1;

	}
	return 0;
	
	

}

void AShooterCharacter::AimingButtonPressed() {
	bAiming = 1;

	
}

void AShooterCharacter::AimingButtonReleased() {
	bAiming = 0;
	
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime) {

	if (bAiming) {
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
	}
	else {
		CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);

	}
	GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);


}

void AShooterCharacter::CalculateCrosshairSpread(float DeltaTime) {
	FVector2D WalkSpeedRange{ 0.f,600.f };
	FVector2D VelocityMultiplierRange{ 0.f,1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

	if (GetCharacterMovement()->IsFalling()) {
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else {
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 32.25f);

	}

	if (bAiming) {
		CrosshairAimFactor= FMath::FInterpTo(CrosshairAimFactor, .6f, DeltaTime, 32.25f);

	}
	else {
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 32.25f);

	}

	if (bFiringBullet) {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, .3f, DeltaTime, 62.25f);

	}
	else {
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 62.25f);

	}

	CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor+CrosshairInAirFactor-CrosshairAimFactor+CrosshairShootingFactor;

}

void AShooterCharacter::StartCrosshairBulletFire() {

	bFiringBullet = 1;
	GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AShooterCharacter::FinishCrosshairBulletFire, ShootTimeDuration);
}

void AShooterCharacter::FinishCrosshairBulletFire() {
	bFiringBullet = 0;

}

void AShooterCharacter::FireButtonPressed() {
	bFireButtonPressed = 1;
	StartFireTimer();
}

void AShooterCharacter::FireButtonReleased() {
	bFireButtonPressed = 0;

}

void AShooterCharacter::StartFireTimer() {
	if (bShouldFire) {
		FireWeapon();
		bShouldFire = 0;

		GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AShooterCharacter::AutoFireReset, AutomaticFireRate);

	}

}

void AShooterCharacter::AutoFireReset() {

	bShouldFire = 1;
	if (bFireButtonPressed) {
		StartFireTimer();
	}
}

bool AShooterCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation) {

	FVector2D ViewPortSize;
	if (GEngine && GEngine->GameViewport) {

		GEngine->GameViewport->GetViewportSize(ViewPortSize);

	}

	FVector2D CrosshairLocation{ ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f };

	FVector CrosshairWorldPosition, CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld) {
		const FVector Start = CrosshairWorldPosition;
		const FVector End = Start + CrosshairWorldDirection * 80000.f;
		OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit) {
			OutHitLocation = OutHitResult.Location;
			return 1;

		}
	}






	return 0;
}

void AShooterCharacter::TraceForItems() {

	if (bShouldTraceForItems) {
		FHitResult ItemTraceResult;
		FVector HitLocatin;
		TraceUnderCrosshairs(ItemTraceResult, HitLocatin);
		if (ItemTraceResult.bBlockingHit) {
			IItemInterface* ItemInterfacex = Cast<IItemInterface>(ItemTraceResult.GetActor());
			if (ItemInterfacex) {
				TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
				ItemInterfacex->SetPickupWidgetVisibility_Implementation(1);
			}
			if (TraceHitItemLastFrame) {
				if (ItemTraceResult.GetActor() != TraceHitItemLastFrame) {
					IItemInterface* ItemInterfacex1 = Cast<IItemInterface>(TraceHitItemLastFrame);

					if (ItemInterfacex1) {
						ItemInterfacex1->SetPickupWidgetVisibility_Implementation(0);
					}
				}
			}

			if (ItemTraceResult.GetActor()) {
				IItemInterface* ItemInterfacex3 = Cast<IItemInterface>(ItemTraceResult.GetActor());
				if(ItemInterfacex3)
				TraceHitItemLastFrame = ItemTraceResult.GetActor();
			}

		}
		
	}
	else if (TraceHitItemLastFrame) {
		IItemInterface* ItemInterfacex2 = Cast<IItemInterface>(TraceHitItemLastFrame);

		if (ItemInterfacex2) {
			ItemInterfacex2->SetPickupWidgetVisibility_Implementation(0);
		}
	}

}

AWeapon* AShooterCharacter::SpawnDefaultWeapon() {

	if (DefaultWeaponClass) {
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		
		
	}

	return nullptr;

}

void AShooterCharacter::EquipWeapon(AWeapon* WeaponToEquip) {

	if (WeaponToEquip) {

		

		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket) {
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
		EquippedWeapon->SetOwner(this);
	}


}

void AShooterCharacter::DropWeapon() {

	if (EquippedWeapon) {
		FDetachmentTransformRules DetachmentRule(EDetachmentRule::KeepWorld, 1);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentRule);
	
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
	
		EquippedWeapon->ThrowWeapon();
	
	}

}

void AShooterCharacter::SelectButtonPressed() {
	
	if (TraceHitItem) {
		TraceHitItem->StartItemCurve(this);
	}

	
}

void AShooterCharacter::SwapWeapon(AWeapon* WeaponToSwap) {

	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}



// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraInterpZoom(DeltaTime);

	CalculateCrosshairSpread(DeltaTime);

	TraceForItems();

	



}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AShooterCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AShooterCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &AShooterCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed,this, &AShooterCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AShooterCharacter::StopJumping);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &AShooterCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Released, this, &AShooterCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("AimButton", IE_Pressed, this, &AShooterCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimButton", IE_Released, this, &AShooterCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("SelectButton", IE_Pressed, this, &AShooterCharacter::SelectButtonPressed);
	


}

float AShooterCharacter::GetCrosshairSpreadMultiplier() const {
	return CrosshairSpreadMultiplier;
}


void AShooterCharacter::IncrementsOverlappedItemCount_Implementation(int32 Amount) {

	if (OverlappedItemCount + Amount <= 0) {
		OverlappedItemCount = 0;
		bShouldTraceForItems = 0;

	}
	else {
		OverlappedItemCount += Amount;
		bShouldTraceForItems = 1;
	}

}

FVector AShooterCharacter::GetCameraInterpLocation() {

	const FVector CameraWorldLocation = FollowCamera->GetComponentLocation();
	const FVector CameraForward = FollowCamera->GetForwardVector();
	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);


	
}

void AShooterCharacter::GetPickupItem(AItem* Item) {

	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon) {
		SwapWeapon(Weapon);
	}



}

void AShooterCharacter::GetsPickupItem_Implementation(AItem* Item) {
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon) {
		SwapWeapon(Weapon);
	}
}

