// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "AudioMixerBlueprintLibrary.h"
#include "MovieSceneSection.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to Not call Tick() every frame (false)
	PrimaryActorTick.bCanEverTick = false;

	// Do we replicate to clients? yes
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh); // Set mesh as root component (dont need to setup attachment then)

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


	// Initialize the pickup widget
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if ( PickupWidget)
	{
		// If true it means the cast succeeded and a blaster character has overlapped
		// And check to make sure pickup widget is valid (not null ptr)
		PickupWidget->SetVisibility(false);
	}

	// if (GetLocalRole() == ENetRole::ROLE_Authority )
	// Same check as HasAuthority, check if we are the server
	if (HasAuthority()) 
		{
			// Only bind callbacks if we are the server!!!
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Overlap if we are a pawn

			// Bind our OnSphereOverlap Function to the area sphere's list of delegates to call back on begin overlap
			AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);

			AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
		}
	
}

// THIS FUNCTION IS ONLY CALLED ON THE SERVER
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor); // Cast the other actor to ABlaster character
	if (BlasterCharacter)
	{
		// True if cast succeeds
		// Set widget visibility only if the other overlapping actor of the blaster character type
		
		// Set the widget for the player that is overlapping the weapon
		// Then the variable for that character will be replicated to the client matching the servers data
		BlasterCharacter->SetOverlappingWeapon(this);

	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor); 
	if (BlasterCharacter)
	{
		// We are no longer overlapping the weapon, so no overlap, set to null
		BlasterCharacter->SetOverlappingWeapon(nullptr);

	}
}

// Result of Server -> Client Replication
// This code Runs on client to adjust weapon state locally
// Same as SetWeaponState But for Clients
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		// Set no collision so we dont keep trying to replicate that an actor is inside the sphere
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Dont need to set owner here since it is replicated from Server -> clients
		break;
	default:
		break;
	}
}

// Only Called on Server
// Updates the weapon state on the server which the variable WeaponState is then replicated to clients
void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		// Set no collision so we dont keep trying to replicate that an actor is inside the sphere
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// Dont need to set owner here since it is replicated from Server -> clients
		break;
	default:
		break;
	}
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::ShowPickupWidget(bool bShowWidget) const
{
	if (PickupWidget)
	{
		//Makesure the widget is valid
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate the variable from server -> Client
	DOREPLIFETIME(AWeapon, WeaponState);
}

