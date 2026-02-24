// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"

#include "HuatsonCharacterMovementComponent.generated.h"

#define UE_API HUATSONGAME_API

class UObject;
struct FFrame;

HUATSONGAME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * FHuatsonCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FHuatsonCharacterGroundInfo
{
	GENERATED_BODY()

	FHuatsonCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};


/**
 * UHuatsonCharacterMovementComponent
 *
 *	The base character movement component class used by this project.
 */
UCLASS(MinimalAPI, Config = Game)
class UHuatsonCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UE_API UHuatsonCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UE_API virtual void SimulateMovement(float DeltaTime) override;

	UE_API virtual bool CanAttemptJump() const override;

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "Huatson|CharacterMovement")
	UE_API const FHuatsonCharacterGroundInfo& GetGroundInfo();

	UE_API void SetReplicatedAcceleration(const FVector& InAcceleration);

	//~UMovementComponent interface
	UE_API virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	UE_API virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

protected:

	UE_API virtual void InitializeComponent() override;

protected:

	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FHuatsonCharacterGroundInfo CachedGroundInfo;

	UPROPERTY(Transient)
	bool bHasReplicatedAcceleration = false;
};

#undef UE_API
