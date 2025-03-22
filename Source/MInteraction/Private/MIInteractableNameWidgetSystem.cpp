// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractableNameWidgetSystem.h"

#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"
#include "MIInteractableNameWidget.h"
#include "MInteractionSettings.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UMIInteractableNameWidgetSystem::Tick(float DeltaTime)
{
	// Remove destroyed interactables
	TArray<TWeakObjectPtr<UObject>> InteractablesToRemove;
	for (auto Itt = WidgetInfoForInteractable.CreateIterator(); Itt; ++Itt)
	{
		const UObject* Interactable = Itt->Key.Get(false);
		if (Interactable == nullptr)
			InteractablesToRemove.Emplace(Itt->Key);
	}

	for (auto InteractableToRemove : InteractablesToRemove)
	{
		FMINameWidgetInstanceInfo InstanceInfo = WidgetInfoForInteractable[InteractableToRemove];
		InstanceInfo.WidgetInstance->RemoveFromParent();
		
		WidgetInfoForInteractable.Remove(InteractableToRemove);
	}

	for (auto Itt = WidgetInfoForInteractable.CreateIterator(); Itt; ++Itt)
	{
		const FMINameWidgetInstanceInfo InstanceInfo = Itt->Value;
		if (InstanceInfo.bDisplayed)
		{
			UObject* Interactable = Itt->Key.Get();
			const FVector AnchorWorldPos = IMIInteractableInterface::Execute_GetObjectNameAnchorPos(Interactable);

			FVector2D WidgetPos;
			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetWorld()->GetFirstPlayerController(),
			                                                           AnchorWorldPos, WidgetPos, true);
			InstanceInfo.WidgetInstance->SetPositionInViewport(WidgetPos, false);
			InstanceInfo.WidgetInstance->SetFocusedState(IMIInteractableInterface::Execute_IsFocused(Interactable));
			
			// Bug in UE - alignement seems to be reset when SetPositionInViewport is called
			InstanceInfo.WidgetInstance->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
		}
	}
}

void UMIInteractableNameWidgetSystem::DisplayInteractableName(UObject* Interactable)
{
	if (ensureMsgf(UMIInteractableFunctionLibrary::IsObjectInteractable(Interactable),
	               TEXT("Trying to display name widget for object that does not implement IHLInteractableInterface")))
	{
		if (!WidgetInfoForInteractable.Contains(Interactable))
		{
			FMINameWidgetInstanceInfo InstanceInfo;

			UMIInteractableNameWidget* WidgetInstance = CreateWidget<UMIInteractableNameWidget>(
				GetWorld(), UMInteractionSettings::Get()->InteractableNameWidgetClass.LoadSynchronous());
			InstanceInfo.WidgetInstance = WidgetInstance;
			InstanceInfo.bDisplayed = false;

			InstanceInfo.WidgetInstance->SetObjectName(
				IMIInteractableInterface::Execute_GetDisplayName(Interactable));

			WidgetInfoForInteractable.Add(Interactable, InstanceInfo);
		}

		FMINameWidgetInstanceInfo* InstanceInfo = WidgetInfoForInteractable.Find(Interactable);
		if (!InstanceInfo->bDisplayed)
		{
			InstanceInfo->bDisplayed = true;
			AddWidgetInstanceToViewport(InstanceInfo->WidgetInstance);
			InstanceInfo->WidgetInstance->StartDisplaying();
		}
	}
}

void UMIInteractableNameWidgetSystem::StopDisplayingInteractableName(UObject* Interactable)
{
	if (WidgetInfoForInteractable.Contains(Interactable))
	{
		FMINameWidgetInstanceInfo* InstanceInfo = WidgetInfoForInteractable.Find(Interactable);
		RemoveWidgetInstanceFromViewport(InstanceInfo->WidgetInstance);
		InstanceInfo->bDisplayed = false;
	}
}

void UMIInteractableNameWidgetSystem::AddWidgetInstanceToViewport(UMIInteractableNameWidget* WidgetInstance)
{
	WidgetInstance->AddToViewport();
}

void UMIInteractableNameWidgetSystem::RemoveWidgetInstanceFromViewport(UMIInteractableNameWidget* WidgetInstance)
{
	WidgetInstance->RemoveFromParent();
}

void UMIInteractableNameWidgetSystem::SetInteractablesInRange(const TSet<UObject*>& InteractablesInRange)
{
	// Find interactables not in range and stop displaying them
	for (const auto InteractableInRangeLastFrame : InteractablesInRangeLastFrame)
	{
		if (!InteractablesInRange.Contains(InteractableInRangeLastFrame))
			StopDisplayingInteractableName(InteractableInRangeLastFrame);
	}

	// Start displaying only new interactables in range
	for (const auto InteractableInRangeThisFrame : InteractablesInRange)
	{
		if (!InteractablesInRangeLastFrame.Contains(InteractableInRangeThisFrame))
			DisplayInteractableName(InteractableInRangeThisFrame);
	}

	InteractablesInRangeLastFrame = InteractablesInRange;
}
