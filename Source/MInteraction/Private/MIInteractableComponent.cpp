// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractableComponent.h"

#include "MInteraction.h"


UMIInteractableComponent::UMIInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bEnabled = true;
}

bool UMIInteractableComponent::Interact_Implementation(AActor* Instigator)
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


bool UMIInteractableComponent::CanInteract_Implementation(AActor* Instigator)
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
	bEnabled = false;
}

void UMIInteractableComponent::DisableInteraction_Implementation()
{
	bEnabled = true;
}

FText UMIInteractableComponent::GetDisplayName_Implementation()
{
	return DisplayName;
}

FText UMIInteractableComponent::GetInteractionVerb_Implementation()
{
	return InteractionVerb;
}

FVector UMIInteractableComponent::GetObjectNameAnchorPos_Implementation()
{
	return GetOwner()->GetActorLocation() + ObjectNameAnchorOffset;
}
