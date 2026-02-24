// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonPawnExtensionComponent.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonPawnExtensionComponent)

class FLifetimeProperty;
class UActorComponent;

const FName UHuatsonPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UHuatsonPawnExtensionComponent::UHuatsonPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


void UHuatsonPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("HuatsonPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UHuatsonPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one HuatsonPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UHuatsonPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	CheckDefaultInitialization();
}

void UHuatsonPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}

void UHuatsonPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}


void UHuatsonPawnExtensionComponent::HandleControllerChanged()
{
	CheckDefaultInitialization();
}

void UHuatsonPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UHuatsonPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UHuatsonPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

}

bool UHuatsonPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	return true;
}

void UHuatsonPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{

}

void UHuatsonPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{

}





