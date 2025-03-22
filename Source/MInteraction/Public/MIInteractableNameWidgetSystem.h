// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MIInteractableNameWidgetSystem.generated.h"

class UMIInteractableNameWidget;

USTRUCT()
struct FMINameWidgetInstanceInfo
{
	GENERATED_BODY()

	UPROPERTY()
	bool bDisplayed = false;

	UPROPERTY()
	UMIInteractableNameWidget* WidgetInstance;
};

/**
 * Manages widgets for interactables in range
 */
UCLASS()
class MINTERACTION_API UMIInteractableNameWidgetSystem : public ULocalPlayerSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	static UMIInteractableNameWidgetSystem* Get(const ULocalPlayer* LocalPlayer)
	{
		return LocalPlayer->GetSubsystem<UMIInteractableNameWidgetSystem>();
	}

	// ~FTickableGameObject

	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; };
	virtual bool IsAllowedToTick() const override { return true; };
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UMIInteractableNameWidgetSystem, STATGROUP_Tickables);
	}

	// ~FTickableGameObject

	void SetInteractablesInRange(const TSet<UObject*>& InteractablesInRange);

protected:
	void DisplayInteractableName(UObject* Interactable);
	void StopDisplayingInteractableName(UObject* Interactable);

	/**
	 * Extend this function for custom add widget handling
	 */
	virtual void AddWidgetInstanceToViewport(UMIInteractableNameWidget* WidgetInstance);

	/**
	 * Extend this function for custom remove widget handling
	 */
	virtual void RemoveWidgetInstanceFromViewport(UMIInteractableNameWidget* WidgetInstance);

protected:
	UPROPERTY()
	TMap<TWeakObjectPtr<UObject>, FMINameWidgetInstanceInfo> WidgetInfoForInteractable;

	UPROPERTY()
	TSet<UObject*> InteractablesInRangeLastFrame;
};
