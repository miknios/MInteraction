// Copyright (c) Miknios. All rights reserved.


#include "MPointSelectorQueryFilter_IsInteractable.h"

#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"

bool UMPointSelectorQueryFilter_IsInteractable::FilterPoint_Impl_Implementation(
	const UObject* ContextObject,
	const FVector& ReferenceLocation,
	const FVector& ReferenceDirection,
	const UMPointSelectorQuerySettingsAsset* QuerySettings,
	const TScriptInterface<IMSelectorQueryPointInterface>& LookAtPoint) const
{
	const UObject* PointObject = LookAtPoint.GetObject();
	if (!IsValid(PointObject))
	{
		return false;
	}

	const AActor* PointActor = [PointObject]
	{
		if (const AActor* Actor = Cast<AActor>(PointObject))
		{
			return Actor;
		}

		const AActor* ActorOuter = PointObject->GetTypedOuter<AActor>();
		return ActorOuter;
	}();

	if (!UMIInteractableFunctionLibrary::IsActorInteractable(PointActor))
	{
		return false;
	}

	const AActor* ContextActor = [ContextObject]
	{
		if (const AActor* Actor = Cast<AActor>(ContextObject))
		{
			return Actor;
		}

		const AActor* ActorOuter = ContextObject->GetTypedOuter<AActor>();
		return ActorOuter;
	}();

	const TScriptInterface<IMIInteractableInterface> PointInteractable =
		UMIInteractableFunctionLibrary::GetInteractableFromActor(PointActor);
	const bool bResult = IMIInteractableInterface::Execute_CanInteract(PointInteractable.GetObject(), ContextActor);
	return bResult;
}
