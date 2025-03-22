// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MIInteractorGatherer_Base.generated.h"

class UMIInteractorComponent;

UCLASS(Abstract, Blueprintable)
class MINTERACTION_API UMIInteractorGatherer_Base : public UObject
{
	GENERATED_BODY()

public:
	virtual void GatherInteractables(UMIInteractorComponent* InteractorComp, TSet<UObject*>& OutInteractablesInRange,
	                                 TOptional<UObject*>& OutInteractableToFocus)
	{
	}
};
