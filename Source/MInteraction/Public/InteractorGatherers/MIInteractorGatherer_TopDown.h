// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MIInteractorComponent.h"
#include "MIInteractorGatherer_Base.h"
#include "MIInteractorGatherer_TopDown.generated.h"

/**
 * 
 */
UCLASS()
class MINTERACTION_API UMIInteractorGatherer_TopDown : public UMIInteractorGatherer_Base
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interactor Gatherer")
	FVector OverlapOffsetFromRoot;

	UPROPERTY(EditDefaultsOnly, Category = "Interactor Gatherer")
	float OverlapRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Interactor Gatherer")
	float OverlapHalfHeight;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Interactor Gatherer")
	float AngleMatchWeight = 1;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0, ClampMax = 1), Category = "Interactor Gatherer")
	float DistanceMatchWeight = 0.5f;

public:
	virtual void GatherInteractables(UMIInteractorComponent* InteractorComp, TSet<UObject*>& OutInteractablesInRange,
	                                                TOptional<UObject*>& OutInteractableToFocus) override;
};
