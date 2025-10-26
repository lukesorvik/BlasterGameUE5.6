// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "AudioMixerBlueprintLibrary.h"
#include "MovieSceneSection.h"
#include "Components/SphereComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to Not call Tick() every frame (false)
	PrimaryActorTick.bCanEverTick = false;

	// Do we replicate to clients? yes
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); // Block all by default
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore); // Ignore collision with players
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // No collision by default
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

	// Once overlap with pawn we want to pickup the weapon
	// But in multiplayer game important things like this should only be done on the server

	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); // Ignore all by default, like it doesnt exist
	// Only enable area sphere collision on server in BeginPlay()
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// if (GetLocalRole() == ENetRole::ROLE_Authority )
	// Same check as HasAuthority, check if we are the server
	if (HasAuthority()) 
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Overlap if we are a pawn
		}
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

