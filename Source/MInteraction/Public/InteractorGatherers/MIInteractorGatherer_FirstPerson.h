// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MIInteractorGatherer_Base.h"
#include "MIInteractorGatherer_FirstPerson.generated.h"

/**
 * 
 */
UCLASS()
class MINTERACTION_API UMIInteractorGatherer_FirstPerson : public UMIInteractorGatherer_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interactor Gatherer")
	float OverlapRadius = 70;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = -1, ClampMax = 1), Category = "Interactor Gatherer")
	float InteractableInRangeDotThreshold = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Interactor Gatherer")
	float FocusDistanceThreshold = 150;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Interactor Gatherer")
	float AngleMatchWeight = 1;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Interactor Gatherer")
	float DistanceMatchWeight = 0.5f;

public:
	virtual void GatherInteractables(UMIInteractorComponent* InteractorComp, TSet<UObject*>& OutInteractablesInRange,
	                                                TOptional<UObject*>& OutInteractableToFocus) override;
};
