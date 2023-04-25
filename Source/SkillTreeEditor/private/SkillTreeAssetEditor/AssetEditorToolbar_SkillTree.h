// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class FAssetEditor_SkillTree;
class FExtender;
class FToolBarBuilder;

/**
 * 
 */
class FAssetEditorToolbar_SkillTree : public TSharedFromThis<FAssetEditorToolbar_SkillTree>
{
public:
	FAssetEditorToolbar_SkillTree(TSharedPtr<FAssetEditor_SkillTree> InSkillTreeEditor)
		: SkillTreeEditor(InSkillTreeEditor) {}

	void AddSkillTreeToolbar(TSharedPtr<FExtender> Extender);

private:
	void FillSkillTreeToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
	/** Pointer back to the blueprint editor tool that owns us */
	TWeakPtr<FAssetEditor_SkillTree> SkillTreeEditor;
};
