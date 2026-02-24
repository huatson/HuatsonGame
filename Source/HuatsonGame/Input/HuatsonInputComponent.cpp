// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonInputComponent.h"

#include "EnhancedInputSubsystems.h"
//#include "Player/HuatsonLocalPlayer.h"
//#include "Settings/HuatsonSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonInputComponent)

class UHuatsonInputConfig;

UHuatsonInputComponent::UHuatsonInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UHuatsonInputComponent::AddInputMappings(const UHuatsonInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UHuatsonInputComponent::RemoveInputMappings(const UHuatsonInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UHuatsonInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
