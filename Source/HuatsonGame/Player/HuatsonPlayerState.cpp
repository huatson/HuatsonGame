// Copyright Epic Games, Inc. All Rights Reserved.

#include "HuatsonPlayerState.h"

#include "Character/HuatsonPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/World.h"
#include "HuatsonLogChannels.h"
#include "HuatsonPlayerController.h"
#include "Net/UnrealNetwork.h"
//#include "Net/Core/PushModel/PushModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonPlayerState)

class AController;
class APlayerState;
class FLifetimeProperty;

const FName AHuatsonPlayerState::NAME_HuatsonAbilityReady("HuatsonAbilitiesReady");

AHuatsonPlayerState::AHuatsonPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MyPlayerConnectionType(EHuatsonPlayerConnectionType::Player)
{

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	MyTeamID = FGenericTeamId::NoTeam;
	MySquadID = INDEX_NONE;
}

void AHuatsonPlayerState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AHuatsonPlayerState::Reset()
{
	Super::Reset();
}

void AHuatsonPlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if (UHuatsonPawnExtensionComponent* PawnExtComp = UHuatsonPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

void AHuatsonPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	//@TODO: Copy stats
}

void AHuatsonPlayerState::OnDeactivated()
{
	bool bDestroyDeactivatedPlayerState = false;

	switch (GetPlayerConnectionType())
	{
	case EHuatsonPlayerConnectionType::Player:
	case EHuatsonPlayerConnectionType::InactivePlayer:
		//@TODO: Ask the experience if we should destroy disconnecting players immediately or leave them around
		// (e.g., for long running servers where they might build up if lots of players cycle through)
		bDestroyDeactivatedPlayerState = true;
		break;
	default:
		bDestroyDeactivatedPlayerState = true;
		break;
	}

	SetPlayerConnectionType(EHuatsonPlayerConnectionType::InactivePlayer);

	if (bDestroyDeactivatedPlayerState)
	{
		Destroy();
	}
}

void AHuatsonPlayerState::OnReactivated()
{
	if (GetPlayerConnectionType() == EHuatsonPlayerConnectionType::InactivePlayer)
	{
		SetPlayerConnectionType(EHuatsonPlayerConnectionType::Player);
	}
}


void AHuatsonPlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		const FGenericTeamId OldTeamID = MyTeamID;

		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyTeamID, this);
		MyTeamID = NewTeamID;
		ConditionalBroadcastTeamChanged(this, OldTeamID, NewTeamID);
	}
	else
	{
		UE_LOG(LogHuatsonTeams, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}
FGenericTeamId AHuatsonPlayerState::GetGenericTeamId() const
{
	return MyTeamID;
}

FOnHuatsonTeamIndexChangedDelegate* AHuatsonPlayerState::GetOnTeamIndexChangedDelegate()
{
	return &OnTeamChangedDelegate;
}



void AHuatsonPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyPlayerConnectionType, SharedParams);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MyTeamID, SharedParams);

	SharedParams.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedViewRotation, SharedParams);
}

FRotator AHuatsonPlayerState::GetReplicatedViewRotation() const
{
	// Could replace this with custom replication
	return ReplicatedViewRotation;
}

void AHuatsonPlayerState::SetReplicatedViewRotation(const FRotator& NewRotation)
{
	if (NewRotation != ReplicatedViewRotation)
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedViewRotation, this);
		ReplicatedViewRotation = NewRotation;
	}
}

AHuatsonPlayerController* AHuatsonPlayerState::GetHuatsonPlayerController() const
{
	return Cast<AHuatsonPlayerController>(GetOwner());
}


void AHuatsonPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}


void AHuatsonPlayerState::SetPlayerConnectionType(EHuatsonPlayerConnectionType NewType)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MyPlayerConnectionType, this);
	MyPlayerConnectionType = NewType;
}

void AHuatsonPlayerState::SetSquadID(int32 NewSquadId)
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, MySquadID, this);

		MySquadID = NewSquadId;
	}
}


void AHuatsonPlayerState::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
	ConditionalBroadcastTeamChanged(this, OldTeamID, MyTeamID);
}

void AHuatsonPlayerState::OnRep_MySquadID()
{
	//@TODO: Let the squad subsystem know (once that exists)
}

void AHuatsonPlayerState::OnRep_PawnData()
{
}
