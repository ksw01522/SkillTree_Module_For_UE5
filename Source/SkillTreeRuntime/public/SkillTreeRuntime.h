// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(SkillTreeRuntime, Log, All);

#define CUSTOMLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
#define RUNTIMELOG(VER, FMT, ...) UE_LOG(SkillTreeRuntime, VER, TEXT("%s %s"), *CUSTOMLOG_CALLINFO, *FString::Printf(FMT, ##__VA_ARGS__) )

class ISkillTreeRuntime : public IModuleInterface
{
public:

	static ISkillTreeRuntime& Get()
	{
		return FModuleManager::LoadModuleChecked<ISkillTreeRuntime>("SkillTreeRuntime");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("SkillTreeRuntime");
	}
};

class FSkillTreeRuntime : public ISkillTreeRuntime
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};






