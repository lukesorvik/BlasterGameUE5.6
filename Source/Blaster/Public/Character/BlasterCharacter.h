// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputMappingContext.h"


#include "BlasterCharacter.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputMapping;


	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

	// Exec means its called from the in game console
	UFUNCTION(Exec)
	void vclip();

	// #Step 2: Need to overide in order to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpHeld(const FInputActionValue& Value);

	void StartCrouch(const FInputActionValue& Value);
	void StopCrouch(const FInputActionValue& Value);
	void CrouchHeld(const FInputActionValue& Value);


private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	//  meta = (AllowPrivateAccess = "true") allows private member to be visible in editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	bool bIsVClipEnabled = false;

	// #Step 1: uproperty replicated to replicate a variable
	// On rep Overlapping weapon will be called on the client when overlapping weapon replicates to that client
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	
	// Called automatically when the variable after the OnRep_ is replicated
	// OnRep_ will pass in the last value before replication as a parameter
	// which is why the parameter must be the same type of the thing being replicated
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeaponBeforeReplication);
	
public:
	void SetOverlappingWeapon(AWeapon* Weapon);


};
