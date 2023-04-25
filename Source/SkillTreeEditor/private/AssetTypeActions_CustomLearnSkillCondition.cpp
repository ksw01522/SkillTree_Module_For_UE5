// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypeActions_CustomLearnSkillCondition.h"
#include "LearnSkillCondition.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions_Dialogue"

FText FAssetTypeActions_CustomLearnSkillCondition::GetName() const
{
	return LOCTEXT("FCustomLearnSkillConditionAssetTypeActionsName", "Custom Learn Skill Condition");
}

UClass* FAssetTypeActions_CustomLearnSkillCondition::GetSupportedClass() const
{
	return ULearnSkillCondition::StaticClass();
}



#undef LOCTEXT_NAMESPACE