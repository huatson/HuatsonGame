// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "Teams/HuatsonTeamAgentInterface.h"

#include "HuatsonPlayerState.generated.h"

#define UE_API HUATSONGAME_API

struct FGenericTeamId;

class AController;
class AHuatsonPlayerController;
class APlayerState;
class FName;
class UObject;
struct FFrame;
struct FGameplayTag;

/** Defines the types of client connected */
UENUM()
enum class EHuatsonPlayerConnectionType : uint8
{
	// An active player
	Player = 0,

	// Spectator connected to a running game
	LiveSpectator,

	// Spectating a demo recording offline
	ReplaySpectator,

	// A deactivated player (disconnected)
	InactivePlayer
};

/**
 * AHuatsonPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS(MinimalAPI, Config = Game)
class AHuatsonPlayerState : public APlayerState, public	IHuatsonTeamAgentInterface
{
	GENERATED_BODY()

public:
	UE_API AHuatsonPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Huatson|PlayerState")
	UE_API AHuatsonPlayerController* GetHuatsonPlayerController() const;

	//~AActor interface
	UE_API virtual void PreInitializeComponents() override;
	UE_API virtual void PostInitializeComponents() override;
	//~End of AActor interface

	//~APlayerState interface
	UE_API virtual void Reset() override;
	UE_API virtual void ClientInitialize(AController* C) override;
	UE_API virtual void CopyProperties(APlayerState* PlayerState) override;
	UE_API virtual void OnDeactivated() override;
	UE_API virtual void OnReactivated() override;
	//~End of APlayerState interface


	//~IHuatsonTeamAgentInterface interface
	UE_API virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	UE_API virtual FGenericTeamId GetGenericTeamId() const override;
	UE_API virtual FOnHuatsonTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of IHuatsonTeamAgentInterface interface

	static UE_API const FName NAME_HuatsonAbilityReady;

	UE_API void SetPlayerConnectionType(EHuatsonPlayerConnectionType NewType);
	EHuatsonPlayerConnectionType GetPlayerConnectionType() const { return MyPlayerConnectionType; }

	/** Returns the Squad ID of the squad the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetSquadId() const
	{
		return MySquadID;
	}

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
	int32 GetTeamId() const
	{
		return GenericTeamIdToInteger(MyTeamID);
	}

	UE_API void SetSquadID(int32 NewSquadID);


	// Gets the replicated view rotation of this player, used for spectating
	UE_API FRotator GetReplicatedViewRotation() const;

	// Sets the replicated view rotation, only valid on the server
	UE_API void SetReplicatedViewRotation(const FRotator& NewRotation);

protected:
	UFUNCTION()
	UE_API void OnRep_PawnData();

private:

	UPROPERTY()
	FOnHuatsonTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY(ReplicatedUsing=OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

	UPROPERTY(ReplicatedUsing=OnRep_MySquadID)
	int32 MySquadID;

	UPROPERTY(Replicated)
	EHuatsonPlayerConnectionType MyPlayerConnectionType;

	UPROPERTY(Replicated)
	FRotator ReplicatedViewRotation;

private:
	UFUNCTION()
	UE_API void OnRep_MyTeamID(FGenericTeamId OldTeamID);

	UFUNCTION()
	UE_API void OnRep_MySquadID();

};

#undef UE_API
