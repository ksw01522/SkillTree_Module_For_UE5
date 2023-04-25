// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillTreeRuntime.h"

DEFINE_LOG_CATEGORY(SkillTreeRuntime);

IMPLEMENT_MODULE( FSkillTreeRuntime, SkillTreeRuntime );

void FSkillTreeRuntime::StartupModule()
{
	// This code will execute after your module is loaded into memory (but after global variables are initialized, of course.)
}


void FSkillTreeRuntime::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}