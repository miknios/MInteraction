// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MIInteractableInterface.h"
#include "Components/ActorComponent.h"
#include "MIInteractableComponent.generated.h"

USTRUCT(BlueprintType)
struct FMIOnInteractedData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* Instigator;

	FMIOnInteractedData()
	{
	}

	FMIOnInteractedData(AActor* Instigator)
		: Instigator(Instigator)
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractedSignature, const FMIOnInteractedData&, OnInteractedData);

/**
 * Component attached to actors that player can interact with common interaction button
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINTERACTION_API UMIInteractableComponent : public UActorComponent, public IMIInteractableInterface
{
	GENERATED_BODY()

public:
	UMIInteractableComponent();

	virtual bool Interact_Implementation(AActor* Instigator) override;
	virtual bool CanInteract_Implementation(AActor* Instigator) override;
	virtual void EnableInteractableFocused_Implementation() override;
	virtual void DisableInteractableFocused_Implementation() override;
	virtual bool IsFocused_Implementation() override;
	virtual void EnableInteraction_Implementation() override;
	virtual void DisableInteraction_Implementation() override;
	virtual FText GetDisplayName_Implementation() override;
	virtual FText GetInteractionVerb_Implementation() override;
	virtual FVector GetObjectNameAnchorPos_Implementation() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	FText InteractionVerb;

	UPROPERTY(EditAnywhere, Category = "Interactable")
	FVector ObjectNameAnchorOffset;

	UPROPERTY(BlueprintAssignable)
	FOnInteractedSignature OnInteractedDelegate;

	UPROPERTY()
	bool bEnabled;

	UPROPERTY()
	bool bFocused;
};
