// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "HuatsonPawnData.generated.h"

#define UE_API HUATSONGAME_API

class APawn;
//class UHuatsonAbilitySet;
//class UHuatsonAbilityTagRelationshipMapping;
class UHuatsonCameraMode;
class UHuatsonInputConfig;
class UObject;


/**
 * UHuatsonPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(MinimalAPI, BlueprintType, Const, Meta = (DisplayName = "Huatson Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UHuatsonPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UE_API UHuatsonPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// Class to instantiate for this pawn (should usually derive from AHuatsonPawn or AHuatsonCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Huatson|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Huatson|Abilities")
	//TArray<TObjectPtr<UHuatsonAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Huatson|Abilities")
	//TObjectPtr<UHuatsonAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Huatson|Input")
	TObjectPtr<UHuatsonInputConfig> InputConfig;

	// Default camera mode used by player controlled pawns.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Huatson|Camera")
	TSubclassOf<UHuatsonCameraMode> DefaultCameraMode;
};

#undef UE_API
