// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BlasterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


// Anim instance exists on each machine for each character
// but it only has access to variables on that machine


public:
	// Similar to beginPlay, called when the anim instance is initialized
	virtual void NativeInitializeAnimation() override;

	// Called every frame to update the animation properties
	// Similar to the tick function
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	// Need meta = (AllowPrivateAccess = "true") to access private variables in Blueprints
	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ABlasterCharacter* BlasterCharacter;

	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(BluePrintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped; // Check if we have a weapon currently equipped
	// Replicated from CombatComponent, used to see if a weapon is equipped
};
