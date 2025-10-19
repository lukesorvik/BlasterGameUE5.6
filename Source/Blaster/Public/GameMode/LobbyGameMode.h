// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * See how many players have connected to the lobby, then once we have enough, travel to the main level
 */
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
