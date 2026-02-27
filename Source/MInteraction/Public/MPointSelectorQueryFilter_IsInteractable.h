// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MPointSelectorQueryFilter.h"
#include "MPointSelectorQueryFilter_IsInteractable.generated.h"

UCLASS(DisplayName = "Is Interactable")
class MINTERACTION_API UMPointSelectorQueryFilter_IsInteractable : public UMPointSelectorQueryFilter_Base
{
	GENERATED_BODY()

protected:
	// UMPointSelectorQueryFilter_Base
	virtual bool FilterPoint_Impl_Implementation(const UObject* ContextObject, const FVector& ReferenceLocation,
	                                             const FVector& ReferenceDirection,
	                                             const UMPointSelectorQuerySettingsAsset* QuerySettings,
	                                             const TScriptInterface<IMSelectorQueryPointInterface>& LookAtPoint) const override;
	// ~ UMPointSelectorQueryFilter_Base
};
