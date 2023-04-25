// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/AssetEditorToolbar_SkillTree.h"
#include "SkillTreeAssetEditor/AssetEditor_SkillTree.h"
#include "SkillTreeAssetEditor/EditorCommands_SkillTree.h"
#include "SkillTreeAssetEditor/SkillTreeEditorStyle.h"
#include "Styling/AppStyle.h"

#define LOCTEXT_NAMESPACE "AssetEditorToolbar_SkillTree"

void FAssetEditorToolbar_SkillTree::AddSkillTreeToolbar(TSharedPtr<FExtender> Extender)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FAssetEditor_SkillTree> SkillTreeEditorPtr = SkillTreeEditor.Pin();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension("Asset", EExtensionHook::After, SkillTreeEditorPtr->GetToolkitCommands(), FToolBarExtensionDelegate::CreateSP(this, &FAssetEditorToolbar_SkillTree::FillSkillTreeToolbar));
	SkillTreeEditorPtr->AddToolbarExtender(ToolbarExtender);

}

void FAssetEditorToolbar_SkillTree::FillSkillTreeToolbar(FToolBarBuilder& ToolbarBuilder)
{
	check(SkillTreeEditor.IsValid());
	TSharedPtr<FAssetEditor_SkillTree> SkillTreeEditorPtr = SkillTreeEditor.Pin();

	ToolbarBuilder.BeginSection("Skill Tree");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_SkillTree::Get().GraphSettings,
			NAME_None,
			LOCTEXT("GraphSettings_Label", "Graph Settings"),
			LOCTEXT("GraphSettings_ToolTip", "Show the Graph Settings"),
			FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.GameSettings"));
	}
	ToolbarBuilder.EndSection();

	ToolbarBuilder.BeginSection("Util");
	{
		ToolbarBuilder.AddToolBarButton(FEditorCommands_SkillTree::Get().AutoArrange,
			NAME_None,
			LOCTEXT("AutoArrange_Label", "Auto Arrange"),
			LOCTEXT("AutoArrange_ToolTip", "Auto arrange nodes, not perfect, but still handy"),
			FSlateIcon(FSkillTreeEditorStyle::GetStyleSetName(), "SkillTreeEditor.AutoArrange"));
	}
	ToolbarBuilder.EndSection();

}

#undef LOCTEXT_NAMESPACE