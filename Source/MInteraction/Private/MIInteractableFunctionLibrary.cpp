// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"

bool UMIInteractableFunctionLibrary::IsInteractable(AActor* Actor)
{
	return Actor->Implements<UMIInteractableInterface>()
		|| Actor->FindComponentByInterface<IMIInteractableInterface>() != nullptr;
}

bool UMIInteractableFunctionLibrary::IsObjectInteractable(UObject* Object)
{
	return Object->Implements<UMIInteractableInterface>();
}

bool UMIInteractableFunctionLibrary::Interact(AActor* Actor)
{
	UObject* InteractableObject = nullptr;
	if (Actor->Implements<UMIInteractableInterface>())
	{
		InteractableObject = Actor;
	}
	else if (IMIInteractableInterface* InteractableComponent =
		Actor->FindComponentByInterface<IMIInteractableInterface>())
	{
		InteractableObject = Cast<UObject>(InteractableComponent);
	}

	if (InteractableObject != nullptr)
	{
		if (IMIInteractableInterface::Execute_CanInteract(InteractableObject, Actor))
			return IMIInteractableInterface::Execute_Interact(InteractableObject, Actor);
	}

	return false;
}

UObject* UMIInteractableFunctionLibrary::GetInteractableFromActor(AActor* Actor)
{
	UObject* ResultingObject = nullptr;

	if (Actor->Implements<UMIInteractableInterface>())
	{
		ResultingObject = Actor;
	}
	else if (IMIInteractableInterface* InteractableComponent =
		Actor->FindComponentByInterface<IMIInteractableInterface>())
	{
		ResultingObject = Cast<UObject>(InteractableComponent);
	}

	return ResultingObject;
}
