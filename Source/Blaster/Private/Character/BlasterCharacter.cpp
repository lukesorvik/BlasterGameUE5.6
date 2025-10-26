// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/BlasterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

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

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f); // go up the stairs bro

	// Setup Overhead text box
	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent); // Attach to root

	// enable crouching
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// // Enable replication
	// bReplicates = true;
}

void ABlasterCharacter::vclip()
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	
	if (MoveComp)
	{
		// Toggle VClip
		if (bIsVClipEnabled)
		{
			// Disable VClip
			bIsVClipEnabled = false;
	
			// Restore capsule collision
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
			// Restore camera collision
			CameraBoom->bDoCollisionTest = true;
	
			// Restore movement
			MoveComp->SetMovementMode(MOVE_Walking);
			
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Flying Disabled"));
		}
		else
		{
			// Enable vclip
			bIsVClipEnabled = true;
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
			CameraBoom->bDoCollisionTest = false;
			MoveComp->SetMovementMode(MOVE_Flying);
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Flying Enabled"));
		}
	}

	//
	// ServerSetVClip_Implementation(bIsVClipEnabled);
	
}

// void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
// {
// 	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
// 	// For Vclip replication
// 	DOREPLIFETIME(ABlasterCharacter, bIsVClipEnabled);
// }

// Called when the game starts or when spawned
void ABlasterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABlasterCharacter::Move(const FInputActionValue& Value)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(3, 15.0f, FColor::Green, FString::Printf(TEXT("Move input: %s"), *Value.ToString()));
	// }
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
	// if (GEngine)
	// {
	// 	// Custom key of 2 so it doesn't overwrite other messages
	// 	GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Green, FString::Printf(TEXT("Look input: %s"), *Value.ToString()));
	// }
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

// void ABlasterCharacter::ServerSetVClip_Implementation(bool bIsVClipEnabledFromClient)
// {
// 	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
//
// 	if (!MoveComp) return;
//
// 	if (bIsVClipEnabledFromClient == false)
// 	{
// 		// Enable VClip
// 		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
// 		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
// 		CameraBoom->bDoCollisionTest = false;
// 		MoveComp->SetMovementMode(MOVE_Flying);
//
// 		if (GEngine)
// 			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Flying Enabled"));
// 	}
// 	else
// 	{
// 		// Disable VClip
// 		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
// 		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
// 		CameraBoom->bDoCollisionTest = true;
// 		MoveComp->SetMovementMode(MOVE_Walking);
//
// 		if (GEngine)
// 			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Flying Disabled"));
// 	}
// }


// Called every frame
void ABlasterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this,  &ABlasterCharacter::StopCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Ongoing, this, &ABlasterCharacter::CrouchHeld);
		
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("EnhancedInputComponent not found!"));
		}
	}

}

