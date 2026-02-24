// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "HuatsonCameraComponent.h"
#include "HuatsonUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonPlayerCameraManager)

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

AHuatsonPlayerCameraManager::AHuatsonPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = HUATSON_CAMERA_DEFAULT_FOV;
	ViewPitchMin = HUATSON_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = HUATSON_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UHuatsonUICameraManagerComponent>(UICameraComponentName);
}

UHuatsonUICameraManagerComponent* AHuatsonPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void AHuatsonPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void AHuatsonPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("HuatsonPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UHuatsonCameraComponent* CameraComponent = UHuatsonCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}

