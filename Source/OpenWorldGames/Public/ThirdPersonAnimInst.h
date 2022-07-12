// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimBPInterface.h"
#include "ThirdPersonAnimInst.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDGAMES_API UThirdPersonAnimInst : public UAnimInstance,public IAnimBPInterface
{
	GENERATED_BODY()
public:
	UThirdPersonAnimInst();

	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

	virtual void UeLog_PrintOutputLog_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class AOpenWorldGamesCharacter* ShooterCharacter;

	/** The speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		float Speed;



};
