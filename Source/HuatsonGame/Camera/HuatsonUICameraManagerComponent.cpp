#include "HuatsonUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "HuatsonPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UHuatsonUICameraManagerComponent::UHuatsonUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}


UHuatsonUICameraManagerComponent* UHuatsonUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (AHuatsonPlayerCameraManager* PCM = Cast<AHuatsonPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCM->GetUICameraComponent();
		}
	}

	return nullptr;
}

void UHuatsonUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UHuatsonUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams /*= FViewTargetTransitionParams()*/)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<AHuatsonPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);

}

bool UHuatsonUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UHuatsonUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{

}

void UHuatsonUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{

}
