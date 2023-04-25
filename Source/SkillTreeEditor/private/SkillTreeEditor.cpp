// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkillTreeEditor.h"
#include "SkillNodeFactory.h"
#include "AssetTypeActions_SkillTree.h"
#include "AssetTypeActions_CustomLearnSkillCondition.h"
#include "SkillTreeAssetEditor/SkillTreeEditorStyle.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "K2Node_Event.h"

#include "LearnSkillCondition.h"
#include "SkillTreeEditorUtilities.h"



DEFINE_LOG_CATEGORY(SkillTreeEditor)
IMPLEMENT_MODULE(FSkillTreeEditor, SkillTreeEditor)

#define LOCTEXT_NAMESPACE "Editor_SkillTree"

void FSkillTreeEditor::StartupModule()
{
	FSkillTreeEditorStyle::Initialize();

	RegisterNodeFactory();
	RegisterCategory();

	RegisterBlueprintEvents();
}

void FSkillTreeEditor::ShutdownModule()
{	
// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}

	UnRegisterNodeFactory();

	FSkillTreeEditorStyle::Shutdown();
}


void FSkillTreeEditor::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

void FSkillTreeEditor::RegisterCategory()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	SkillTreeAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("SkillTree")), LOCTEXT("SkillTreeAssetCategory", "SkillTree"));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_SkillTree(SkillTreeAssetCategoryBit)));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_CustomLearnSkillCondition(SkillTreeAssetCategoryBit)));
}

void FSkillTreeEditor::RegisterNodeFactory()
{
	GraphPanelNodeFactory_SkillTree = MakeShareable(new FGraphPanelNodeFactory_SkillTree());
	FEdGraphUtilities::RegisterVisualNodeFactory(GraphPanelNodeFactory_SkillTree);
}

void FSkillTreeEditor::UnRegisterNodeFactory()
{
	if (GraphPanelNodeFactory_SkillTree.IsValid())
	{
		FEdGraphUtilities::UnregisterVisualNodeFactory(GraphPanelNodeFactory_SkillTree);
		GraphPanelNodeFactory_SkillTree.Reset();
	}
}

void FSkillTreeEditor::RegisterBlueprintEvents()
{
	FKismetEditorUtilities::RegisterOnBlueprintCreatedCallback(
		this,
		ULearnSkillCondition::StaticClass(),
		FKismetEditorUtilities::FOnBlueprintCreated::CreateRaw(this, &FSkillTreeEditor::HandleNewCustomLearnSkillConditionBlueprintCreated)
	);
}

void FSkillTreeEditor::HandleNewCustomLearnSkillConditionBlueprintCreated(UBlueprint* Blueprint)
{

	if (!Blueprint || Blueprint->BlueprintType != BPTYPE_Normal)
	{
		return;
	}

	Blueprint->bForceFullEditor = true;
	UK2Node_Event* EventNode = FSkillTreeEditorUtilities::BlueprintGetOrAddEvent(
		Blueprint,
		GET_FUNCTION_NAME_CHECKED(ULearnSkillCondition, IsCanLearn),
		ULearnSkillCondition::StaticClass()
	);
	if (EventNode)
	{
		Blueprint->LastEditedDocuments.Add(EventNode->GetGraph());
	}
	
}










#undef LOCTEXT_NAMESPACE
