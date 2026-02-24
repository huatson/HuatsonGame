// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonAnimInstance.h"
#include "Character/HuatsonCharacter.h"
#include "Character/HuatsonCharacterMovementComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonAnimInstance)


UHuatsonAnimInstance::UHuatsonAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


#if WITH_EDITOR
EDataValidationResult UHuatsonAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);
	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UHuatsonAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UHuatsonAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const AHuatsonCharacter* Character = Cast<AHuatsonCharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	UHuatsonCharacterMovementComponent* CharMoveComp = CastChecked<UHuatsonCharacterMovementComponent>(Character->GetCharacterMovement());
	const FHuatsonCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}

