// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterComponents/CombatComponent.h"

#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.
	// DEFAULT TO OFF ENABLE IF WE NEED SOMETHING TO TICK EVERY FRAME
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UCombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// Replicate this variable 
	DOREPLIFETIME(UCombatComponent, EquippedWeapon)
	DOREPLIFETIME(UCombatComponent, bAiming)
}

/////////////////////////////////////////
///
///

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (BlasterCharacter == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	// Updates the WeaponState which is replicated to clients
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = BlasterCharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, BlasterCharacter->GetMesh());
	}

	// Set owner of weapon to the pawn that equipped it
	EquippedWeapon->SetOwner(BlasterCharacter);
}

// Either called on Server Client or Client
// Show aiming immediately, do not wait on RPC to show animation
// Other players seeing the aim late is ok
void UCombatComponent::SetAiming(bool bIsAiming)
{
	// Update local bool so we can update animation immediately
	// (if we are server, it will be replicated automatically to clients)
	bAiming = bIsAiming;

	
	// Dont need to Check if Authority or not
	// https://cedric-neukirchen.net/docs/multiplayer-compendium/remote-procedure-calls
	// If we're on the server, it'll just run on the server.
	// It won't run on any other machine, but that's okay because we're setting a replicated variable so the value will replicate to all clients.
	// But if we're on a client and we call this, it'll be executed on the server.

	// if (!BlasterCharacter->HasAuthority())
	// {
	// If we are not the server, Notify other clients that we just Aimed
	ServerSetAiming(bIsAiming);

	// }
}


void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}
