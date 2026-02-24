// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GenericTeamAgentInterface.h"
#include "UObject/Object.h"

#include "UObject/WeakObjectPtr.h"
#include "HuatsonTeamAgentInterface.generated.h"

#define UE_API HUATSONGAME_API

template <typename InterfaceType> class TScriptInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHuatsonTeamIndexChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);

inline int32 GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)ID;
}

inline FGenericTeamId IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}

/** Interface for actors which can be associated with teams */
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UHuatsonTeamAgentInterface : public UGenericTeamAgentInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IHuatsonTeamAgentInterface : public IGenericTeamAgentInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual FOnHuatsonTeamIndexChangedDelegate* GetOnTeamIndexChangedDelegate() { return nullptr; }

	static UE_API void ConditionalBroadcastTeamChanged(TScriptInterface<IHuatsonTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID);
	
	FOnHuatsonTeamIndexChangedDelegate& GetTeamChangedDelegateChecked()
	{
		FOnHuatsonTeamIndexChangedDelegate* Result = GetOnTeamIndexChangedDelegate();
		check(Result);
		return *Result;
	}
};

#undef UE_API
