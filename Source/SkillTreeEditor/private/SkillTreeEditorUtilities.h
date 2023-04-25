// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UEdGraph;
class FSlateRect;
class UK2Node_Event;

class FSkillTreeEditorUtilities
{
public:
	// Same as BlueprintGetOrAddFunction but only for an overriden event
	static UK2Node_Event* BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);

	// Same as BlueprintGetOrAddEvent but does not add it
	static UK2Node_Event* BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature);
};
