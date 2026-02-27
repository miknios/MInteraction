// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractableComponent.h"

#include "MInteraction.h"


UMIInteractableComponent::UMIInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bEnabled = true;
}

bool UMIInteractableComponent::Interact_Implementation(const AActor* Instigator)
{
	if (Execute_CanInteract(this, Instigator))
	{
		if (OnInteractedDelegate.IsBound())
		{
			OnInteractedDelegate.Broadcast(FMIOnInteractedData(Instigator));
			return true;
		}

		UE_LOG(LogMInteraction, Error, TEXT("Cannot interact with actor without action bound to OnInteractedDelegate"))
	}

	return false;
}


bool UMIInteractableComponent::CanInteract_Implementation(const AActor* Instigator)
{
	return bEnabled;
}

void UMIInteractableComponent::EnableInteractableFocused_Implementation()
{
	bFocused = true;
}

void UMIInteractableComponent::DisableInteractableFocused_Implementation()
{
	bFocused = false;
}

bool UMIInteractableComponent::IsFocused_Implementation()
{
	return bFocused;
}

void UMIInteractableComponent::EnableInteraction_Implementation()
{
	bEnabled = true;
}

void UMIInteractableComponent::DisableInteraction_Implementation()
{
	bEnabled = false;
}
