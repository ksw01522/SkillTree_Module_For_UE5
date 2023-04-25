// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/AppStyle.h"
/**
 * 
 */
class FEditorCommands_SkillTree : public TCommands<FEditorCommands_SkillTree>
{
public:
	FEditorCommands_SkillTree()
		:	TCommands<FEditorCommands_SkillTree>("SkillTreeEditor", NSLOCTEXT("Contexts", "SkillTreeEditor", "Skill Tree Editor"), NAME_None, FAppStyle::GetAppStyleSetName())
	{}

	TSharedPtr<FUICommandInfo> GraphSettings;
	TSharedPtr<FUICommandInfo> AutoArrange;

	virtual void RegisterCommands() override;
};
