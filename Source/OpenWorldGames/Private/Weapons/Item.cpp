// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Item.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/ShooterCharacterInterface.h"
#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Curves/CurveVector.h"


// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);




	PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
	PickupWidget->SetupAttachment(GetRootComponent());

	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(GetRootComponent());






}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	PickupWidget->SetVisibility(0);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	SetItemProperties(ItemState);

	InitializeCustomDepth();

	StartPulseTimer();





}

void AItem::SetItemProperties(EItemState State) {


	switch (State) {
	case EItemState::EIS_Pickup:

		PickupWidget->SetVisibility(0);

		ItemMesh->SetSimulatePhysics(0);
		ItemMesh->SetVisibility(1);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		break;
	case EItemState::EIS_EquipInterping:
		PickupWidget->SetVisibility(0);

		ItemMesh->SetSimulatePhysics(0);
		ItemMesh->SetVisibility(1);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;
	case EItemState::EIS_PickedUp:
		PickupWidget->SetVisibility(false);
		
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		break;
	case EItemState::EIS_Equipped:
		ItemMesh->SetSimulatePhysics(0);
		ItemMesh->SetVisibility(1);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


		break;
	case EItemState::EIS_Falling:

		ItemMesh->SetSimulatePhysics(1);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);




		break;
	case EItemState::EIS_MAX:


		break;
	default:


		break;
	}











}

void AItem::FinishInterping() {
	bInterping = 0;
	if (Character) {
		Character->GetPickupItem(this);
	}
	SetActorScale3D(FVector(1.f));

	DisableGlowMaterial();
	
	bCanChangeCustomDepth = 1;

	DisableCustomDepth();


}

void AItem::ItemInterp(float DeltaTime) {
	if (!bInterping) { return; }

	if (Character && ItemZCurve) {
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		FVector ItemLocation = ItemInterpStartLocation;
		const FVector CameraInterpLocation = Character->GetCameraInterpLocation();
		const FVector ItemToCamera = FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z);
		float DeltaZ = ItemToCamera.Size();
		
		const FVector CurrentLocation = GetActorLocation();
		const float InterpXValue = FMath::FInterpTo(CurrentLocation.X, CameraInterpLocation.X, DeltaTime, 30.f);
		const float InterpYValue = FMath::FInterpTo(CurrentLocation.Y, CameraInterpLocation.Y, DeltaTime, 30.f);

		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, 1, nullptr, ETeleportType::TeleportPhysics);

		const FRotator CameraRotation = Character->GetFollowCamera()->GetComponentRotation();
		FRotator ItemRotation{ 0.f,CameraRotation.Yaw + InterpInitialYawOffset,0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve) {
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);

			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
		





	}








}

void AItem::EnableCustomDepth() {
	if(bCanChangeCustomDepth)
	ItemMesh->SetRenderCustomDepth(1);

}

void AItem::DisableCustomDepth() {
	if(bCanChangeCustomDepth)
	ItemMesh->SetRenderCustomDepth(0);


}

void AItem::SetCustomDepthx1_Implementation(bool bEnable) {

	if (bEnable) {
		EnableCustomDepth();
	}
	else {
		DisableCustomDepth();
	}


}

void AItem::InitializeCustomDepth() {
	DisableCustomDepth();
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ItemInterp(DeltaTime);

	UpdatePulse();



}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if (OtherActor) {
		IShooterCharacterInterface* ShooterCharacterInterface = Cast<IShooterCharacterInterface>(OtherActor);
		if (ShooterCharacterInterface) {
			ShooterCharacterInterface->IncrementsOverlappedItemCount_Implementation(1);
		}
		

	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

	if (OtherActor) {
		IShooterCharacterInterface* ShooterCharacterInterface = Cast<IShooterCharacterInterface>(OtherActor);
		if (ShooterCharacterInterface) {
			ShooterCharacterInterface->IncrementsOverlappedItemCount_Implementation(-1);
		}


	}
}

void AItem::OnConstruction(const FTransform& Transform) {

	if (MaterialInstance) {
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance,this);
	
		ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
	
	}
	EnableGlowMaterial();



}

void AItem::EnableGlowMaterial() {

	if (DynamicMaterialInstance) {

		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 0);
	}

}

void AItem::DisableGlowMaterial() {

	if (DynamicMaterialInstance) {

		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), 1);
	}


}

void AItem::UpdatePulse() {

	if (ItemState != EItemState::EIS_Pickup)return;

	const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(PulseTimer);

	if (PulseCurve) {

		const FVector CurveValue = PulseCurve->GetVectorValue(ElapsedTime);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), CurveValue.X*GlowAmount);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), CurveValue.Y * FresnelExponent);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), CurveValue.Z * FresnelReflectFraction);



	
	
	
	}





}

void AItem::ResetPulseTimer() {

	StartPulseTimer();

}

void AItem::StartPulseTimer() {
	if (ItemState == EItemState::EIS_Pickup) {
		GetWorldTimerManager().SetTimer(PulseTimer, this, &AItem::ResetPulseTimer, PulseCurveTime);
	}
}

void AItem::SetPickupWidgetVisibility_Implementation(bool bVisible) {

	PickupWidget->SetVisibility(bVisible);

}

void AItem::SetCollisionsToIgnore_Implementation() {
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	
}

void AItem::SetItemState(EItemState State) {
	ItemState = State;
	SetItemProperties(ItemState);




}

void AItem::StartItemCurve(AShooterCharacter* Char) {

	Character = Char;

	if (PickupSound) UGameplayStatics::PlaySound2D(this, PickupSound);


	ItemInterpStartLocation = GetActorLocation();
	bInterping = 1;
	SetItemState(EItemState::EIS_EquipInterping);
	GetWorldTimerManager().SetTimer(ItemInterpTimer,this,&AItem::FinishInterping,ZCurveTime);

	const float CameraRotationYaw = Character->GetFollowCamera()->GetComponentRotation().Yaw;
	const float ItemRotationYaw = GetActorRotation().Yaw;
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;

	bCanChangeCustomDepth = 0;
}




