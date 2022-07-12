// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ItemInterface.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRarity :uint8 {

	EIR_Damaged UMETA(DisplayName="Damaged")
	, EIR_Common UMETA(DisplayName = "Common")
	, EIR_UnCommon UMETA(DisplayName = "UnCommon")
	, EIR_Rare UMETA(DisplayName = "Rare")
	, EIR_Legendary UMETA(DisplayName = "Legendary")

	, EIR_MAX UMETA(DisplayName = "DefaultMAX")

};

UENUM(BlueprintType)
enum class EItemState :uint8 {

	EIS_Pickup UMETA(DisplayName = "Pickup")
	, EIS_EquipInterping UMETA(DisplayName = "EquipInterping")
	, EIS_PickedUp UMETA(DisplayName = "PickedUp")
	, EIS_Equipped UMETA(DisplayName = "Equipped")
	, EIS_Falling UMETA(DisplayName = "Falling")

	, EIS_MAX UMETA(DisplayName = "DefaultMAX")

};







UCLASS()
class OPENWORLDGAMES_API AItem : public AActor,public IItemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	void SetItemProperties(EItemState State);

	void FinishInterping();

	void ItemInterp(float DeltaTime);




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* AreaSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		FString ItemName=FString("Default");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		int32 ItemCount=0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		EItemRarity ItemRarity = EItemRarity::EIR_Common;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		EItemState ItemState=EItemState::EIS_Pickup;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class UCurveFloat* ItemZCurve;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		FVector ItemInterpStartLocation=FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		FVector CameraTargetLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		bool bInterping = 0;

	FTimerHandle ItemInterpTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float ZCurveTime=.7f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class AShooterCharacter* Character;

	float ItemInterpX = 0.f;
	float ItemInterpY=0.f;

	float InterpInitialYawOffset=0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		UCurveFloat* ItemScaleCurve;





public:


	virtual void SetPickupWidgetVisibility_Implementation(bool bVisible) override;

	virtual void SetCollisionsToIgnore_Implementation() override;

	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	void StartItemCurve(AShooterCharacter* Char);









};
