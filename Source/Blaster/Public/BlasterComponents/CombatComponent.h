// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


// Extension of BlasterCharacter.h , just to seperate some of the logic from the blaster character
// Since BlasterCharacter.h is a friend it can access all parts of this class as if it were it's own
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCombatComponent();
	friend class ABlasterCharacter;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Forward declare in the param to tell compiler the class exists somewhere
	// Only called By Server, Equips the Weapon
	void EquipWeapon(class AWeapon* WeaponToEquip);

	// #Step 2: Need to overide in order to replicate
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Replicate EquippedWeapon property for each pawn, Server -> Clients, so all clients match authoritative
	UPROPERTY(Replicated)
	class AWeapon* EquippedWeapon;
	
	class ABlasterCharacter* BlasterCharacter;

	

public:
};
