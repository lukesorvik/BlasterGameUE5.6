// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Weapon.h"

#include "AudioMixerBlueprintLibrary.h"
#include "MovieSceneSection.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

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
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap); // Overlap if we are a pawn

			// Bind our OnSphereOverlap Function to the area sphere's list of delegates to call back on begin overlap
			AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		}
	
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	// Set widget visability only if the other overlapping actor is the blaster character
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(OtherActor); // Cast the other actor to ABlaster character
	if (BlasterCharacter)
	{
		// Set the player's character
		BlasterCharacter->SetOverlappingWeapon(this);
		

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

