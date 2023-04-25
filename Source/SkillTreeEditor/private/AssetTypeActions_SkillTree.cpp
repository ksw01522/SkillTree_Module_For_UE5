// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_SkillTree.h"
#include "SkillTreeEditor.h"
#include "SkillTreeAssetEditor/AssetEditor_SkillTree.h"
#include "SkillTree.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_SkillTree"

FAssetTypeActions_SkillTree::FAssetTypeActions_SkillTree(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{
}

FText FAssetTypeActions_SkillTree::GetName() const
{
	return LOCTEXT("FSkillTreeAssetTypeActionsName", "Custom Graph");
}

FColor FAssetTypeActions_SkillTree::GetTypeColor() const
{
	return FColor(129, 196, 115);
}

UClass* FAssetTypeActions_SkillTree::GetSupportedClass() const
{
	return USkillTree::StaticClass();
}

void FAssetTypeActions_SkillTree::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (USkillTree* SkillTree = Cast<USkillTree>(*ObjIt))
		{
			TSharedRef<FAssetEditor_SkillTree> NewGraphEditor(new FAssetEditor_SkillTree());
			NewGraphEditor->InitSkillTreeAssetEditor(Mode, EditWithinLevelEditor, SkillTree);
		}
	}
}

uint32 FAssetTypeActions_SkillTree::GetCategories()
{
	return MyAssetCategory;
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE