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
	
	/*
	*The simplified order for an actor’s setup is:

	Constructor
	→ Sets up default subobjects and initializes default values.
	(Runs in both editor and runtime.)

	OnConstruction()
	→ Called when the actor is placed or changed in the editor, or spawned in game.
	(Often used for editor preview logic.)

	PreInitializeComponents()

	InitializeComponents()
	→ Each component (e.g., StaticMesh, Audio, etc.) runs its own InitializeComponent().

	PostInitializeComponents()
	→ All components are fully initialized and registered with the world.

	BeginPlay()
	→ The actor officially “starts playing” in the game world.
	*/

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

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EquipAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AimAction;

	// Exec means its called from the in game console
	UFUNCTION(Exec)
	void vclip();

	

	// #Step 2: Need to overide in order to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// Called after all of an actor's components have been initialized, but before gameplay begins
	// So all components are guarenteed to exist, have been registered, and are ready for you to safely access or modify — but the game hasn’t started “ticking” yet.
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpHeld(const FInputActionValue& Value);

	void StartCrouch(const FInputActionValue& Value);
	void StopCrouch(const FInputActionValue& Value);
	void CrouchHeld(const FInputActionValue& Value);
	void EquipButtonPressed(const FInputActionValue& Value);
	void AimButtonPressed(const FInputActionValue& Value);
	void AimButtonReleased(const FInputActionValue& Value);


private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	//  meta = (AllowPrivateAccess = "true") allows private member to be visible in editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverHeadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_vClip)
	bool bIsVClipEnabled = false;

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	// #Step 1: uproperty replicated to replicate a variable
	// On rep Overlapping weapon will be called on the client when overlapping weapon replicates to that client
	// https://dev.epicgames.com/documentation/en-us/unreal-engine/replicate-actor-properties-in-unreal-engine#addareplicatedusingproperty
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;
	
	// Called automatically when the variable after the OnRep_ is replicated
	// OnRep_ will pass in the last value before replication as a parameter
	// which is why the parameter must be the same type of the thing being replicated
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeaponBeforeReplication);

	// When the server replicates bIsVClipEnabled for a pawn we call this function in response
	// Use this to update the actor's collision properties based on their new vclip status
	// Server -> Client, clients run this function in response (not called on server since server doesnt replicate to itself)
	UFUNCTION()
	void OnRep_vClip();

	// Reliable RPC : guarenteed to be executed (distributed system, need ack like tcp), Costly, dont want in something like the tick function
	// Unreliable RPC: Not guarenteed, packets can be dropped
	// RPC Called on Client When the equip button is pressed -> Calls Equip Weapon On Server
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	// RPC executed on the server, Clients or the server calls, can the Server exuctes this
	UFUNCTION(Server, Reliable)
	void ServerVClipRPC();


	// Toggle player collision settings locally depending on bIsvclipEnabled
	void EnableVclipSettings();
	
public:
	// Setters
	void SetOverlappingWeapon(AWeapon* Weapon);

	// Getters
	bool IsWeaponEquipped();
	bool IsAiming();
	
};
