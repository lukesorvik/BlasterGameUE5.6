// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/OverHeadWidget.h"

#include "Components/TextBlock.h"

void UOverHeadWidget::SetDisplayText(const FString& TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverHeadWidget::ShowPlayerNetRole(APawn* InPawn)
{

	// Remote role = role on the server
	// ENetRole RemoteRole = InPawn->GetRemoteRole();
	// FString Role;
	// switch (RemoteRole)
	// {
	// case ENetRole::ROLE_Authority:
	// 	Role = "Authority";
	// 	break;
	// case ENetRole::ROLE_AutonomousProxy:
	// 	Role = "Autonomous Proxy";
	// 	break;
	// case ENetRole::ROLE_SimulatedProxy:
	// 	Role = "Simulated Proxy";
	// 	break;
	// case ENetRole::ROLE_None:
	// 	Role = "None";
	// 	break;
	// default:
	// 	Role = "Unknown";
	// 	break;
	// }
	//
	// FString RemoteRoleString = FString::Printf(TEXT("Local Role: %s"), *Role);
	// SetDisplayText(RemoteRoleString);
	
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
}

// void UOverHeadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
// {
// 	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
// 	RemoveFromParent();
// }
void UOverHeadWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RemoveFromParent();
	
}
