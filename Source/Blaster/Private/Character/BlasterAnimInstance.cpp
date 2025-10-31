// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterAnimInstance.h"

#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// On startup get the blaster character associated
void UBlasterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);


	// Check Blaster character we got on initialization
	if (BlasterCharacter == nullptr)
	{
		// If first tick we dont have the character
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter)
	{
		//Ptr evaluates true if != nullptr

		// Update the animation properties every tick
		// Set Anim Instance Variables based on Character
		
		// Get the lateral speed of the character from velocity
		FVector Velocity = BlasterCharacter->GetVelocity();
		Velocity.Z = 0; // Don't care about vertical speed
		Speed = Velocity.Size();

		// Is the character in air?
		bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
		
		// Is the character accelerating?
		// The Size() method calculates the magnitude of this vector. If the magnitude is greater than 0.f, it means the character is actively accelerating, and the boolean bIsAccelerating is set to true. Otherwise, it is set to false.
		// Ternary operator instead of if else
		bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
		bIsCrouching = BlasterCharacter->bIsCrouched;
		bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
		bAiming = BlasterCharacter->IsAiming();
	}
}
