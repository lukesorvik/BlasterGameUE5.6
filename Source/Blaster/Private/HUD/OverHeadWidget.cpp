// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverHeadWidget.h"

#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverHeadWidget::SetDisplayText(const FString& TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{

	ENetRole LocalRole = InPawn->GetLocalRole();
	FString Role;
	switch (LocalRole)
	{
	case ENetRole::ROLE_Authority:
		Role = "Authority";
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = "Autonomous Proxy";
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = "Simulated Proxy";
		break;
	case ENetRole::ROLE_None:
		Role = "None";
		break;
	default:
		Role = "Unknown";
		break;
	}
	
	FString LocalRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	SetDisplayText(LocalRoleString);

	// // STEAM NAME FUNCTIONALITY, ONLY SHOWS HOST NAME, DOES NOT WORK FOR OTHER PLAYERS
	// if (!InPawn) return;
	//
	// APlayerState* PlayerState = InPawn->GetPlayerState();
	// if (!PlayerState) return;
	//
	// // Get the player's name (Steam name if using Steam)
	// FString PlayerName = PlayerState->GetPlayerName();
	//
	// // Display the name
	// SetDisplayText(PlayerName);
}


void UOverHeadWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RemoveFromParent();
	
}
