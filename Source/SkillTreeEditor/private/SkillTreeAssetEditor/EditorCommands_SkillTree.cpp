// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/EditorCommands_SkillTree.h"

#define LOCTEXT_NAMESPACE "EditorCommands_SkillTree"

void FEditorCommands_SkillTree::RegisterCommands()
{
	UI_COMMAND(GraphSettings, "Graph Settings", "Graph Settings", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AutoArrange, "Auto Arrange", "Auto Arrange", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
