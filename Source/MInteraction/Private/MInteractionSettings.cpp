// Fill out your copyright notice in the Description page of Project Settings.


#include "MInteractionSettings.h"

UMInteractionSettings* UMInteractionSettings::Get()
{
	return CastChecked<UMInteractionSettings>(StaticClass()->GetDefaultObject());
}
