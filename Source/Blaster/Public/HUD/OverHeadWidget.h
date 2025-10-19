// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverHeadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Associates this c++ widget with the UMG widget's text block
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(const FString& TextToDisplay);

	UFUNCTION(BlueprintCallable)
	// Given a pointer to a character, show their net role above their head
	void ShowPlayerNetRole(APawn* InPawn);

protected:
	// Called when the level is removed from the world, or we transition to a different level
	// Allows us to do some cleanup if needed, like removing the widget from viewport
	// Deprecated? use NativeDestruct() instead https://forums.unrealengine.com/t/where-is-uuserwidget-onlevelremovedfromworld-in-5-1/692215
	// virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

	virtual void NativeConstruct() override;
};
