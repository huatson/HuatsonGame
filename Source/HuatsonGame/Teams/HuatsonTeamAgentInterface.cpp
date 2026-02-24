// Copyright Epic Games, Inc. All Rights Reserved.

#include "Teams/HuatsonTeamAgentInterface.h"

#include "HuatsonLogChannels.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HuatsonTeamAgentInterface)

UHuatsonTeamAgentInterface::UHuatsonTeamAgentInterface(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void IHuatsonTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IHuatsonTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
	if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID);
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(LogHuatsonTeams, Verbose, TEXT("[%s] %s assigned team %d"), *GetClientServerContextString(ThisObj), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}

