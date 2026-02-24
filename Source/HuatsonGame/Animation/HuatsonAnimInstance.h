// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "HuatsonAnimInstance.generated.h"

class UAbilitySystemComponent;


/**
 * UHuatsonAnimInstance
 *
 *	The base game animation instance class used by this project.
 */
UCLASS(Config = Game)
class UHuatsonAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UHuatsonAnimInstance(const FObjectInitializer& ObjectInitializer);

protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:


	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};
