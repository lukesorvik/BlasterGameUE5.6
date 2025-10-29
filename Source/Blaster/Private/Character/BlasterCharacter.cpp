// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "BlasterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

// Sets default values
ABlasterCharacter::ABlasterCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 85.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 600.f; // Jump velocity lmao 1000f is pretty high
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a subobject of this class named CameraBoom with the type USpringArmComponent
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 600.f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller rotations
	// CameraBoom->bDoCollisionTest = false;


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f); // go up the stairs bro

	// Setup Overhead text box
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent); // Attach to root

	// enable crouching
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Enable replication
	bReplicates = true;

	// Initialize combat component
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
}


// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Vclip can be called on either server on client
void ABlasterCharacter::vclip()
{
	if (GEngine)
	{
		const FString Authority = HasAuthority() ? TEXT("Server") : TEXT("Client");
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
		                                 FString::Printf(TEXT("VClip called - %s"), *Authority));
	}

	// Either Case of Client or server calling this function: call RPC to Server
	// I think if we are server it just doesnt send the rpc and executes on the server
	ServerVClipRPC();
}


void ABlasterCharacter::ServerVClipRPC_Implementation()
{
	if (GEngine)
	{
		const FString Authority = HasAuthority() ? TEXT("Server") : TEXT("Client");
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
		                                 FString::Printf(TEXT("RPC called - %s"), *Authority));
	}

	if (bIsVClipEnabled)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Flying Disabled"));
		bIsVClipEnabled = false;
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Flying Enabled"));
		bIsVClipEnabled = true;
	}

	EnableVclipSettings();
}

// Toggle player settings depending on Vclip
void ABlasterCharacter::EnableVclipSettings()
{
	if (GEngine)
	{
		const FString Authority = HasAuthority() ? TEXT("Server") : TEXT("Client");
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
		                                 FString::Printf(TEXT("EnableVclipSettings called - %s"), *Authority));
	}

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	if (MoveComp)
	{
		if (bIsVClipEnabled == false)
		{
			// Restore capsule collision
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
			// Restore camera collision
			CameraBoom->bDoCollisionTest = true;
			// Restore movement
			MoveComp->SetMovementMode(MOVE_Walking);
		}
		else
		{
			// Enable vclip
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
			CameraBoom->bDoCollisionTest = false;
			MoveComp->SetMovementMode(MOVE_Flying);
		}
	}
}


void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// #STEP 3: Add variable to replicate
	// DOREPLIFETIME(Class, variable within class to replicate)

	// Replicate the weapon we are overlapping with to all clients
	// DOREPLIFETIME(ABlasterCharacter, OverlappingWeapon)

	// Only replicate to the client that owns the blaster character (COND_OWERONLy == I OWN THE PAWN)
	DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly)

	// Replicate this variable to everyone not just the person who owns the pawn
	// So if person b's variable changes everyone knows about it
	DOREPLIFETIME(ABlasterCharacter, bIsVClipEnabled)
}

void ABlasterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		// Initialize combat component variable to point to this blaster character
		Combat->BlasterCharacter = this;
	}
}


// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABlasterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ABlasterCharacter::JumpHeld(const FInputActionValue& Value)
{
	if (bIsVClipEnabled)
	{
		// Fly up
		FVector FlyDirection = FVector::UpVector; // Default to upward movement
		float FlySpeed = 5.f; // Default fly speed

		// Add movement input in Z axis
		AddMovementInput(FlyDirection, FlySpeed);
	}
}

void ABlasterCharacter::StartCrouch(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	CameraBoom->SocketOffset.Z = -20.f; // Adjust as needed
	Super::Crouch();
}

void ABlasterCharacter::StopCrouch(const FInputActionValue& Value)
{
	CameraBoom->SocketOffset.Z = 0.f;
	if (GetCharacterMovement()->IsFalling())
	{
		return;
	}
	Super::UnCrouch();
}


void ABlasterCharacter::CrouchHeld(const FInputActionValue& Value)
{
	if (bIsVClipEnabled)
	{
		// Fly down
		FVector FlyDirection = FVector::DownVector; // Default to upward movement
		float FlySpeed = 5.f; // Default fly speed

		// Add movement input in Z axis
		AddMovementInput(FlyDirection, FlySpeed);
	}
}

void ABlasterCharacter::EquipButtonPressed(const FInputActionValue& Value)
{
	if (Combat && HasAuthority() && OverlappingWeapon)
	{
		// If combat component is valid && we are the server && and OverlappingWeapon Exists

		Combat->EquipWeapon(OverlappingWeapon);
	}
	else if (Combat && !HasAuthority() && OverlappingWeapon)
	{
		// We are not the server so send the rpc
		ServerEquipButtonPressed();
	}
}

void ABlasterCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeaponBeforeReplication)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1,10.0f, FColor::Black, TEXT("Replicated"));
	// }
	// This should only be called two times - when we enter the sphere, and when we exit
	// On exit, LastWeapon = true, Overlapping weapon = false(null), so hide
	// On enter, LastWeapon= false(null), OverlappingWeapon = true, so show
	// NEVER CALLED ON SERVER
	// ONLY REPLICATE SERVER -> CLIENT
	// THIS FUNCTION IS A CALLBACK FOR WHEN THE CLIENT RECIEVES DATA FROM THE SERVER FOR OVERLAPPINGWEAPON
	if (OverlappingWeapon)
	{
		// Make sure the Weapon Replicated correctly (null can be replicated as well)
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeaponBeforeReplication)
	{
		// If the last value was not null
		// Hide the widget
		LastWeaponBeforeReplication->ShowPickupWidget(false);
	}
}

// Called on client after server updates Vclip bool for a character
// Update local settings on the client depending on a pawn's vclip bool
void ABlasterCharacter::OnRep_vClip()
{
	if (GEngine)
	{
		const FString Authority = HasAuthority() ? TEXT("Server") : TEXT("Client");
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
										 FString::Printf(TEXT("OnRep_Vclip Called - %s"), *Authority));
	}
	EnableVclipSettings();
}


// OnSphereOverlapp only gets called on the server, which is calling this function
// Therefore the machine running this code IS THE SERVER
// ONLY RUN ON THE SERVER
// BECAUSE OF THIS WE NEED TO ADD LOGIC THAT WOULD BE OnRep_OverlappingWeapon HERE, BUT ONLY FOR THE SERVER
// SINCE OnRep_OverlappingWeapon WILL NOT BE CALLED ON THE SERVER
void ABlasterCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		// If the OverlappingWeapon = True, that means we are about to replace it with null
		// So essentially OverlappingWeapon = LastWeapon
		// So disable widget since the server does not get replicated, so OnRep_OverlappingWeapon will not be called for the server!
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;

	if (IsLocallyControlled() && OverlappingWeapon)
	{
		// If the server is controlling this character, and the Weapon is not null
		// then show the widget,
		// since replication only works server->client
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

bool ABlasterCharacter::IsWeaponEquipped()
{
	// We have access to the combat component(since we are friend in combatcomponent.h)

	// return true if combact component exists, and the equipped weapon is not null
	// Bellow code is safe:
	// in normal single-threaded C++ the left operand of && is evaluated first and the operator short‑circuits, so the second expression is not evaluated when the first is null; you won't get a null‑pointer dereference in that case.
	// Notes and caveats:
	// If another thread can set the pointer to null between the check and the access, a race condition could still cause a crash. Game code in Unreal typically runs on the game thread, so this is usually not a concern.
	// For UObject/UActorComponent pointers, the pointer can be non‑null while the object is marked PendingKill. Use IsValid(...) (Unreal helper) to guard against that.
	return (Combat && Combat->EquippedWeapon);
}

void ABlasterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
	return;
}


// Called to bind functionality to input
void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// Initialize Enhanced Input actions, and bind to the functions
	//https://dev.epicgames.com/community/learning/tutorials/eD13/unreal-engine-enhanced-input-in-ue5
	//https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine?application_version=5.5

	// Stolen from third person template

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
			//https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine
			// When you add an Input Mapping Context to the Enhanced Input subsystem, you can also give it priority. If you have multiple contexts mapped to the same Input Action, then, when the Input Action is triggered, the context with the highest priority will be considered and the others ignored.
			// Give higher priority to mouse look thank using gamepad
			//Subsystem->AddMappingContext(InputMappingMouse, 1);
		}
	}

	// Set up action bindings
	// The input action listeners will callback our function we bind when the event is triggered
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Ongoing, this, &ABlasterCharacter::JumpHeld);


		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABlasterCharacter::Look);

		// Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABlasterCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this,
		                                   &ABlasterCharacter::StopCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Ongoing, this, &ABlasterCharacter::CrouchHeld);

		//Equip
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this,
		                                   &ABlasterCharacter::EquipButtonPressed);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("EnhancedInputComponent not found!"));
		}
	}
}
