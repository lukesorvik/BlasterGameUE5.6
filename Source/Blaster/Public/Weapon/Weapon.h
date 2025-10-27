// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType) // allows us to use the enum as a type in blueprints


// Custom types to represent the weaponstate
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName="Initial State"), // Weapon sitting in world, not equipped
	EWS_Equipped UMETA(DisplayName="Equipped"),
	EWS_Dropped UMETA(DisplayName="Dropped"), // Collision enabled

	EWS_MAX UMETA(DisplayName="DefaultMax") // Max used to check how many enum constants are in this enum, can check the value of EWS_MAX
	
};


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Pass true or false if we want to show the pickup widget
	void ShowPickupWidget(const bool bShowWidget) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Declare as ufunction so we can use this as a delegate for the onOverlap callback
	// Only want overlap functions to happen on the server
	// Bind to the server only
	// virtual so child funcs can override
	// Parameters matching delegate we bind to are required for an overlap function
	// Called when Something overlaps with the weapon's collision capsule
	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Callback for delegate of weapon when the overlap ends
	// Should be called on the server only
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class USphereComponent* AreaSphere; 

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	



};
