// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/ShooterCharacter.h"





AAmmo::AAmmo() {

	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>("AmmoMesh");
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(GetRootComponent());
	GetAreaSphere()->SetupAttachment(GetRootComponent());
	GetPickupWidget()->SetupAttachment(GetRootComponent());

	AmmoCollisionSphere = CreateDefaultSubobject<USphereComponent>("AmmoCollisionSphere");
	AmmoCollisionSphere->SetupAttachment(GetRootComponent());
	AmmoCollisionSphere->SetSphereRadius(50.f);



}

void AAmmo::Tick(float DeltaTime) {

	Super::Tick(DeltaTime);




}

void AAmmo::BeginPlay() {

	Super::BeginPlay();

	AmmoCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnAreaSphereOverlap);




}

void AAmmo::SetItemProperties(EItemState State) {

	Super::SetItemProperties( State);

	switch (State) {
	case EItemState::EIS_Pickup:

		
		AmmoMesh->SetSimulatePhysics(0);
		AmmoMesh->SetVisibility(1);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		
		break;
	case EItemState::EIS_EquipInterping:
		
		AmmoMesh->SetSimulatePhysics(0);
		AmmoMesh->SetVisibility(1);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		

		break;
	case EItemState::EIS_PickedUp:


		break;
	case EItemState::EIS_Equipped:
		AmmoMesh->SetSimulatePhysics(0);
		AmmoMesh->SetVisibility(1);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		

		break;
	case EItemState::EIS_Falling:

		AmmoMesh->SetSimulatePhysics(1);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

		



		break;
	case EItemState::EIS_MAX:


		break;
	default:


		break;
	}






}

void AAmmo::OnAreaSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if (OtherActor) {
		auto OverlappCharacter=Cast<AShooterCharacter>(OtherActor);
		if (OverlappCharacter) {

			StartItemCurve(OverlappCharacter);
			AmmoCollisionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}


	}


}

void AAmmo::EnableCustomDepth() {

	AmmoMesh->SetRenderCustomDepth(1);

}

void AAmmo::DisableCustomDepth() {
	AmmoMesh->SetRenderCustomDepth(0);


}
