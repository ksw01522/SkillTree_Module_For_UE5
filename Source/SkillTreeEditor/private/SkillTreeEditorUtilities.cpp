// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEditorUtilities.h"
#include "K2Node_Event.h"
#include "Kismet2/KismetEditorUtilities.h"


#include "Toolkits/IToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "Templates/Casts.h"
#include "Containers/Queue.h"
#include "EdGraphNode_Comment.h"
#include "FileHelpers.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Engine/Blueprint.h"


UK2Node_Event* FSkillTreeEditorUtilities::BlueprintGetOrAddEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
{

	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	// Find existing event
	if (UK2Node_Event* EventNode = BlueprintGetEvent(Blueprint, EventName, EventClassSignature))
	{
		return EventNode;
	}

	// Create a New Event
	if (Blueprint->UbergraphPages.Num())
	{
		int32 NodePositionY = 0;
		UK2Node_Event* NodeEvent = FKismetEditorUtilities::AddDefaultEventNode(
			Blueprint,
			Blueprint->UbergraphPages[0],
			EventName,
			EventClassSignature,
			NodePositionY
		);
		NodeEvent->SetEnabledState(ENodeEnabledState::Enabled);
		NodeEvent->NodeComment = "";
		NodeEvent->bCommentBubbleVisible = false;
		return NodeEvent;
	}
	
	return nullptr;
}

UK2Node_Event* FSkillTreeEditorUtilities::BlueprintGetEvent(UBlueprint* Blueprint, FName EventName, UClass* EventClassSignature)
{

	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return nullptr;
	}

	TArray<UK2Node_Event*> AllEvents;
	//FBlueprintEditorUtils::GetAllNodesOfClass<UK2Node_Event>(Blueprint, AllEvents);
	
	for (UK2Node_Event* EventNode : AllEvents)
	{
		if (EventNode->bOverrideFunction && EventNode->EventReference.GetMemberName() == EventName)
		{
			return EventNode;
		}
	}
	
	return nullptr;
}
