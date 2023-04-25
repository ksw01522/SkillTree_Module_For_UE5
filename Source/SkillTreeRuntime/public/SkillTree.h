// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "SkillTree.generated.h"

/**
 * 
 */
class USkillNode;

UCLASS(BlueprintType)
class SKILLTREERUNTIME_API USkillTree : public UObject
{
	GENERATED_BODY()
public:
	USkillTree();
	virtual ~USkillTree() {}

// EdGraph
public:
#if WITH_EDITORONLY_DATA
	UPROPERTY()
		class UEdGraph* EdGraph;
#endif


// SkillTree
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dialogue")
	FGameplayTagContainer SkillTags;

	UPROPERTY()
	TArray<USkillNode*> RootNodes;

	UPROPERTY()
	TArray<USkillNode*> AllNodes;

public:
#if WITH_EDITOR
	virtual void ClearGraph();

#endif



public:
};
