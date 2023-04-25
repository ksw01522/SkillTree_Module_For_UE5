// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeFactory.h"

#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "SkillTreeFactory"
/*
class FAssetClassParentFilter : public IClassViewerFilter
{
public:
	FAssetClassParentFilter()
		: DisallowedClassFlags(CLASS_None), bDisallowBlueprintBase(false)
	{}

	// All children of these classes will be included unless filtered out by another setting. 
	TSet< const UClass* > AllowedChildrenOfClasses;

	// Disallowed class flags.
	EClassFlags DisallowedClassFlags;

	// Disallow blueprint base classes. 
	bool bDisallowBlueprintBase;

	virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bAllowed = !InClass->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;

		if (bAllowed && bDisallowBlueprintBase)
		{
			if (FKismetEditorUtilities::CanCreateBlueprintOfClass(InClass))
			{
				return false;
			}
		}

		return bAllowed;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if (bDisallowBlueprintBase)
		{
			return false;
		}

		return !InUnloadedClassData->HasAnyClassFlags(DisallowedClassFlags)
			&& InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
	}
};


bool USkillTreeFactory::ConfigureProperties()
{
	// nullptr the SkillTreeClass so we can check for selection
	SkillTreeClass = nullptr;

	// Load the classviewer module to display a class picker
	FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

	// Fill in options
	FClassViewerInitializationOptions Options;
	Options.Mode = EClassViewerMode::ClassPicker;

#if ENGINE_MAJOR_VERSION < 5
	TSharedPtr<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilter = Filter;
#else // #if ENGINE_MAJOR_VERSION < 5
	TSharedRef<FAssetClassParentFilter> Filter = MakeShareable(new FAssetClassParentFilter);
	Options.ClassFilters.Add(Filter);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5

	Filter->DisallowedClassFlags = CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_HideDropDown;
	Filter->AllowedChildrenOfClasses.Add(USkillTreeSession::StaticClass());

	const FText TitleText = LOCTEXT("CreateSkillTreeAssetOptions", "Pick Custom Graph Class");
	UClass* ChosenClass = nullptr;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, USkillTreeSession::StaticClass());

	if (bPressedOk)
	{
		SkillTreeClass = ChosenClass;
	}

	return bPressedOk;
}
*/

USkillTreeFactory::USkillTreeFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = USkillTree::StaticClass();
}

UObject* USkillTreeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	/*
	if (SkillTreeClass != nullptr)
	{
		return NewObject<USkillTreeSession>(InParent, SkillTreeClass, Name, Flags | RF_Transactional);
	}
	else
	{
		check(Class->IsChildOf(UGenericGraph::StaticClass()));
		return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
	}
	*/
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

FText USkillTreeFactory::GetDisplayName() const
{
	return LOCTEXT("FactoryName", "새로운 스킬트리");
}

FString USkillTreeFactory::GetDefaultNewAssetName() const
{
	return "NewSkillTree";
}

#undef LOCTEXT_NAMESPACE