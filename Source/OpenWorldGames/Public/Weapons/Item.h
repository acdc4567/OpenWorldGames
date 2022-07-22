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


	virtual void SetItemProperties(EItemState State);

	void FinishInterping();

	void ItemInterp(float DeltaTime);

	
	virtual void InitializeCustomDepth();








public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnConstruction(const FTransform& Transform) override;

	void EnableGlowMaterial();
	void DisableGlowMaterial();
	
	void UpdatePulse();
	void StartPulseTimer();





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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class USoundCue* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		USoundCue* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		int32 MaterialIndex=0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		UMaterialInstanceDynamic* DynamicMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* MaterialInstance;

	bool bCanChangeCustomDepth = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		class UCurveVector* PulseCurve;

	FTimerHandle PulseTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float PulseCurveTime=5.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float GlowAmount=150.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float FresnelExponent = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ItemProperties, meta = (AllowPrivateAccess = "true"))
		float FresnelReflectFraction = 4.f;

	void ResetPulseTimer();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconBackground;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
		UTexture2D* IconItem;






public:


	virtual void SetPickupWidgetVisibility_Implementation(bool bVisible) override;

	virtual void SetCollisionsToIgnore_Implementation() override;

	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

	FORCEINLINE USoundCue* GetPickupSound() const { return PickupSound; }

	FORCEINLINE USoundCue* GetEquipSound() const { return EquipSound; }

	void StartItemCurve(AShooterCharacter* Char);

	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }

	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }

	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	virtual void EnableCustomDepth();

	virtual void DisableCustomDepth();

	virtual void SetCustomDepthx1_Implementation(bool bEnable) override;

};
