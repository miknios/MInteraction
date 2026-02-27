// Fill out your copyright notice in the Description page of Project Settings.

#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"

bool UMIInteractableFunctionLibrary::IsActorInteractable(const AActor* Actor)
{
	if (Actor == nullptr)
	{
		return false;
	}

	if (Actor->Implements<UMIInteractableInterface>())
	{
		return true;
	}

	const IMIInteractableInterface* InteractableInterface = Actor->FindComponentByInterface<IMIInteractableInterface>();
	const bool bResult = InteractableInterface != nullptr;
	return bResult;
}

bool UMIInteractableFunctionLibrary::IsObjectInteractable(UObject* Object)
{
	if (AActor* Actor = Cast<AActor>(Object))
	{
		return IsActorInteractable(Actor);
	}

	const bool bResult = Object->Implements<UMIInteractableInterface>();
	return bResult;
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

TScriptInterface<IMIInteractableInterface> UMIInteractableFunctionLibrary::GetInteractableFromActor(const AActor* Actor)
{
	if (Actor->Implements<UMIInteractableInterface>())
	{
		return const_cast<AActor*>(Actor);
	}

	if (IMIInteractableInterface* InteractableComponent =
		Actor->FindComponentByInterface<IMIInteractableInterface>())
	{
		return InteractableComponent->_getUObject();
	}

	return nullptr;
}

AActor* UMIInteractableFunctionLibrary::GetActorFromInteractable(TScriptInterface<IMIInteractableInterface> Interactable)
{
	UObject* InteractableObject = Interactable.GetObject();
	if (!IsValid(InteractableObject))
	{
		return nullptr;
	}

	if (AActor* ActorThis = Cast<AActor>(InteractableObject))
	{
		return ActorThis;
	}

	AActor* ActorOuter = Cast<AActor>(InteractableObject->GetOuter());
	return ActorOuter;
}
