// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "Camera/HuatsonCameraAssistInterface.h"
#include "GameFramework/PlayerController.h"
#include "Teams/HuatsonTeamAgentInterface.h"

#include "HuatsonPlayerController.generated.h"

#define UE_API HUATSONGAME_API

struct FGenericTeamId;

class AActor;
class UObject;
class AHuatsonPlayerState;
class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class UObject;
class UPlayer;
struct FFrame;

namespace EEndPlayReason { enum Type : int; }


/**
 * AHuatsonPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(MinimalAPI, Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class AHuatsonPlayerController : public APlayerController, public IHuatsonCameraAssistInterface, public IHuatsonTeamAgentInterface
{
	GENERATED_BODY()

public:

	UE_API AHuatsonPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Huatson|PlayerController")
	UE_API AHuatsonPlayerState* GetHuatsonPlayerState() const;

	//~AActor interface
	UE_API virtual void PreInitializeComponents() override;
	UE_API virtual void BeginPlay() override;
	UE_API virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	UE_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of AActor interface

	//~AController interface
	UE_API virtual void OnPossess(APawn* InPawn) override;
	UE_API virtual void OnUnPossess() override;
	UE_API virtual void InitPlayerState() override;
	UE_API virtual void CleanupPlayerState() override;
	UE_API virtual void OnRep_PlayerState() override;
	//~End of AController interface

	//~APlayerController interface
	UE_API virtual void ReceivedPlayer() override;
	UE_API virtual void PlayerTick(float DeltaTime) override;
	UE_API virtual void SetPlayer(UPlayer* InPlayer) override;
	UE_API virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	UE_API virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
	UE_API virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UE_API virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

	//~IHuatsonCameraAssistInterface interface
	UE_API virtual void OnCameraPenetratingTarget() override;
	//~End of IHuatsonCameraAssistInterface interface
	

	//~ILyraTeamAgentInterface interface
	UE_API virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	UE_API virtual FGenericTeamId GetGenericTeamId() const override;
	UE_API virtual FOnHuatsonTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() override;
	//~End of ILyraTeamAgentInterface interface

	UFUNCTION(BlueprintCallable, Category = "Huatson|Character")
	UE_API void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Huatson|Character")
	UE_API bool GetIsAutoRunning() const;

private:
	UPROPERTY()
	FOnHuatsonTeamIndexChangedDelegate OnTeamChangedDelegate;

	UPROPERTY()
	TObjectPtr<APlayerState> LastSeenPlayerState;


protected:
	// Called when the player state is set or cleared
	UE_API virtual void OnPlayerStateChanged();

private:
	void BroadcastOnPlayerStateChanged();

protected:

	//~APlayerController interface

	//~End of APlayerController interface

	UE_API void OnStartAutoRun();
	UE_API void OnEndAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnStartAutoRun"))
	UE_API void K2_OnStartAutoRun();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnEndAutoRun"))
	UE_API void K2_OnEndAutoRun();

	bool bHideViewTargetPawnNextFrame = false;
};


#undef UE_API
