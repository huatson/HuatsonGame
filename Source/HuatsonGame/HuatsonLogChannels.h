// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

HUATSONGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogHuatson, Log, All);
HUATSONGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogHuatsonExperience, Log, All);
HUATSONGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogHuatsonAbilitySystem, Log, All);
HUATSONGAME_API DECLARE_LOG_CATEGORY_EXTERN(LogHuatsonTeams, Log, All);

HUATSONGAME_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
