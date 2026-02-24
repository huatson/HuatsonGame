// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonHeroComponent.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Logging/MessageLog.h"
#include "HuatsonLogChannels.h"
#include "EnhancedInputSubsystems.h"
#include "Player/HuatsonPlayerController.h"
#include "Player/HuatsonPlayerState.h"
//#include "Player/HuatsonLocalPlayer.h"
#include "Character/HuatsonPawnExtensionComponent.h"
#include "Character/HuatsonPawnData.h"
#include "Character/HuatsonCharacter.h"
//#include "AbilitySystem/HuatsonAbilitySystemComponent.h"
#include "Input/HuatsonInputConfig.h"
#include "Input/HuatsonInputComponent.h"
#include "Camera/HuatsonCameraComponent.h"
#include "HuatsonGameplayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "PlayerMappableInputConfig.h"
#include "Camera/HuatsonCameraMode.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonHeroComponent)

#if WITH_EDITOR
#include "Misc/UObjectToken.h"
#endif	// WITH_EDITOR

namespace HuatsonHero
{
	static const float LookYawRate = 300.0f;
	static const float LookPitchRate = 165.0f;
};

const FName UHuatsonHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UHuatsonHeroComponent::NAME_ActorFeatureName("Hero");

UHuatsonHeroComponent::UHuatsonHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityCameraMode = nullptr;
	bReadyToBindInputs = false;
}

void UHuatsonHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		UE_LOG(LogHuatson, Error, TEXT("[UHuatsonHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint."));

#if WITH_EDITOR
		if (GIsEditor)
		{
			static const FText Message = NSLOCTEXT("HuatsonHeroComponent", "NotOnPawnError", "has been added to a blueprint whose base class is not a Pawn. To use this component, it MUST be placed on a Pawn Blueprint. This will cause a crash if you PIE!");
			static const FName HeroMessageLogName = TEXT("HuatsonHeroComponent");

			FMessageLog(HeroMessageLogName).Error()
				->AddToken(FUObjectToken::Create(this, FText::FromString(GetNameSafe(this))))
				->AddToken(FTextToken::Create(Message));

			FMessageLog(HeroMessageLogName).Open();
		}
#endif
	}
	else
	{
		// Register with the init state system early, this will only work if this is a game world
		RegisterInitStateFeature();
	}
}

bool UHuatsonHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	APawn* Pawn = GetPawn<APawn>();

	if (!CurrentState.IsValid() && DesiredState == HuatsonGameplayTags::InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == HuatsonGameplayTags::InitState_Spawned && DesiredState == HuatsonGameplayTags::InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<AHuatsonPlayerState>())
		{
			return false;
		}

		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();

			const bool bHasControllerPairedWithPS = (Controller != nullptr) && \
				(Controller->PlayerState != nullptr) && \
				(Controller->PlayerState->GetOwner() == Controller);

			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
		}

		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		const bool bIsBot = Pawn->IsBotControlled();

		if (bIsLocallyControlled && !bIsBot)
		{
			AHuatsonPlayerController* HuatsonPC = GetController<AHuatsonPlayerController>();

			// The input component and local player is required when locally controlled.
			if (!Pawn->InputComponent || !HuatsonPC || !HuatsonPC->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == HuatsonGameplayTags::InitState_DataAvailable && DesiredState == HuatsonGameplayTags::InitState_DataInitialized)
	{
		// Wait for player state and extension component
		AHuatsonPlayerState* HuatsonPS = GetPlayerState<AHuatsonPlayerState>();

		return HuatsonPS && Manager->HasFeatureReachedInitState(Pawn, UHuatsonPawnExtensionComponent::NAME_ActorFeatureName, HuatsonGameplayTags::InitState_DataInitialized);
	}
	else if (CurrentState == HuatsonGameplayTags::InitState_DataInitialized && DesiredState == HuatsonGameplayTags::InitState_GameplayReady)
	{
		// TODO add ability initialization checks?
		return true;
	}

	return false;
}

void UHuatsonHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == HuatsonGameplayTags::InitState_DataAvailable && DesiredState == HuatsonGameplayTags::InitState_DataInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		AHuatsonPlayerState* HuatsonPS = GetPlayerState<AHuatsonPlayerState>();
		if (!ensure(Pawn && HuatsonPS))
		{
			return;
		}

		const UHuatsonPawnData* PawnData = nullptr;

		if (UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			PawnData = PawnExtComp->GetPawnData<UHuatsonPawnData>();

			// The player state holds the persistent data for this player (state that persists across deaths and multiple pawns).
			// The ability system component and attribute sets live on the player state.
			//PawnExtComp->InitializeAbilitySystem(HuatsonPS->GetHuatsonAbilitySystemComponent(), HuatsonPS);
		}

		if (AHuatsonPlayerController* HuatsonPC = GetController<AHuatsonPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}

		// Hook up the delegate for all pawns, in case we spectate later
		if (PawnData)
		{
			if (UHuatsonCameraComponent* CameraComponent = UHuatsonCameraComponent::FindCameraComponent(Pawn))
			{
				CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
			}
		}
	}
}

void UHuatsonHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == UHuatsonPawnExtensionComponent::NAME_ActorFeatureName)
	{
		if (Params.FeatureState == HuatsonGameplayTags::InitState_DataInitialized)
		{
			// If the extension component says all all other components are initialized, try to progress to next state
			CheckDefaultInitialization();
		}
	}
}

void UHuatsonHeroComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { HuatsonGameplayTags::InitState_Spawned, HuatsonGameplayTags::InitState_DataAvailable, HuatsonGameplayTags::InitState_DataInitialized, HuatsonGameplayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UHuatsonHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(UHuatsonPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Notifies that we are done spawning, then try the rest of initialization
	ensure(TryToChangeInitState(HuatsonGameplayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UHuatsonHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UHuatsonHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	//const UHuatsonLocalPlayer* LP = Cast<UHuatsonLocalPlayer>(PC->GetLocalPlayer());
	//check(LP);
	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UHuatsonPawnData* PawnData = PawnExtComp->GetPawnData<UHuatsonPawnData>())
		{
			if (const UHuatsonInputConfig* InputConfig = PawnData->InputConfig)
			{
				for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
				{
					if (UInputMappingContext* IMC = Mapping.InputMapping.LoadSynchronous())
					{
						if (Mapping.bRegisterWithSettings)
						{
							if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
							{
								Settings->RegisterInputMappingContext(IMC);
							}

							FModifyContextOptions Options = {};
							Options.bIgnoreAllPressedKeysUntilRelease = false;
							// Actually add the config to the local player							
							Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
						}
					}
				}

				// The Huatson Input Component has some additional functions to map Gameplay Tags to an Input Action.
				// If you want this functionality but still want to change your input component class, make it a subclass
				// of the UHuatsonInputComponent or modify this component accordingly.
				UHuatsonInputComponent* HuatsonIC = Cast<UHuatsonInputComponent>(PlayerInputComponent);
				if (ensureMsgf(HuatsonIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UHuatsonInputComponent or a subclass of it.")))
				{
					// Add the key mappings that may have been set by the player
					HuatsonIC->AddInputMappings(InputConfig, Subsystem);

					// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
					// be triggered directly by these input actions Triggered events. 
					TArray<uint32> BindHandles;
					HuatsonIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

					HuatsonIC->BindNativeAction(InputConfig, HuatsonGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
					HuatsonIC->BindNativeAction(InputConfig, HuatsonGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
					HuatsonIC->BindNativeAction(InputConfig, HuatsonGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
					HuatsonIC->BindNativeAction(InputConfig, HuatsonGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
					HuatsonIC->BindNativeAction(InputConfig, HuatsonGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
				}
			}
		}
	}

	if (ensure(!bReadyToBindInputs))
	{
		bReadyToBindInputs = true;
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UHuatsonHeroComponent::AddAdditionalInputConfig(const UHuatsonInputConfig* InputConfig)
{
	TArray<uint32> BindHandles;

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		UHuatsonInputComponent* HuatsonIC = Pawn->FindComponentByClass<UHuatsonInputComponent>();
		if (ensureMsgf(HuatsonIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UHuatsonInputComponent or a subclass of it.")))
		{
			HuatsonIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
		}
	}
}

void UHuatsonHeroComponent::RemoveAdditionalInputConfig(const UHuatsonInputConfig* InputConfig)
{
	//@TODO: Implement me!
}

bool UHuatsonHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
}

void UHuatsonHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	//if (const APawn* Pawn = GetPawn<APawn>())
	//{
	//	if (const UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	//	{
	//		if (UHuatsonAbilitySystemComponent* HuatsonASC = PawnExtComp->GetHuatsonAbilitySystemComponent())
	//		{
	//			HuatsonASC->AbilityInputTagPressed(InputTag);
	//		}
	//	}
	//}
}

void UHuatsonHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	//if (const UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	//{
	//	if (UHuatsonAbilitySystemComponent* HuatsonASC = PawnExtComp->GetHuatsonAbilitySystemComponent())
	//	{
	//		HuatsonASC->AbilityInputTagReleased(InputTag);
	//	}
	//}
}

void UHuatsonHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	// If the player has attempted to move again then cancel auto running
	if (AHuatsonPlayerController* HuatsonController = Cast<AHuatsonPlayerController>(Controller))
	{
		HuatsonController->SetIsAutoRunning(false);
	}

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UHuatsonHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UHuatsonHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const UWorld* World = GetWorld();
	check(World);

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X * HuatsonHero::LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * HuatsonHero::LookPitchRate * World->GetDeltaSeconds());
	}
}

void UHuatsonHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	if (AHuatsonCharacter* Character = GetPawn<AHuatsonCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void UHuatsonHeroComponent::Input_AutoRun(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (AHuatsonPlayerController* Controller = Cast<AHuatsonPlayerController>(Pawn->GetController()))
		{
			// Toggle auto running
			Controller->SetIsAutoRunning(!Controller->GetIsAutoRunning());
		}
	}
}

TSubclassOf<UHuatsonCameraMode> UHuatsonHeroComponent::DetermineCameraMode() const
{
	if (AbilityCameraMode)
	{
		return AbilityCameraMode;
	}

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UHuatsonPawnData* PawnData = PawnExtComp->GetPawnData<UHuatsonPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UHuatsonHeroComponent::SetAbilityCameraMode(TSubclassOf<UHuatsonCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (CameraMode)
	{
		AbilityCameraMode = CameraMode;
		AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
	}
}

void UHuatsonHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
	if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
	{
		AbilityCameraMode = nullptr;
		AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
	}
}

