// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomLearnSkillConditionFactory.h"
#include "LearnSkillCondition.h"
#include "Kismet2/KismetEditorUtilities.h"


#define LOCTEXT_NAMESPACE "CustomLearnSkillCondition"

UCustomLearnSkillConditionFactory::UCustomLearnSkillConditionFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = ULearnSkillCondition::StaticClass();
}

UObject* UCustomLearnSkillConditionFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (SupportedClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(SupportedClass))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), SupportedClass ? FText::FromString(SupportedClass->GetName()) : NSLOCTEXT("UnrealEd", "Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(NSLOCTEXT("UnrealEd", "CannotCreateBlueprintFromClass", "Cannot create a blueprint based on the class '{ClassName}'."), Args));
		return nullptr;
	}

	return FKismetEditorUtilities::CreateBlueprint(
		SupportedClass,
		InParent,
		Name,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		NAME_None
	);
}

FText UCustomLearnSkillConditionFactory::GetDisplayName() const
{
	return LOCTEXT("Custom Learn Skill Condition Factory Name", "새로운 스킬 배우기 조건 만들기");
}

#undef LOCTEXT_NAMESPACE