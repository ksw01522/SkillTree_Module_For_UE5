// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTreeEdge.generated.h"

/**
 * 
 */
 class USkillTree;
 class USkillNode;
 class UAbilitySystemComponent;

UCLASS()
class SKILLTREERUNTIME_API USkillTreeEdge : public UObject
{
	GENERATED_BODY()

public:
	USkillTreeEdge();
	virtual ~USkillTreeEdge() {}

	UPROPERTY()
	USkillTree* InTree;

	UPROPERTY()
	USkillNode* StartNode;

	UPROPERTY()
	USkillNode* EndNode;

	UPROPERTY(EditDefaultsOnly, Category = "SkillTreeEdge")
	FLinearColor EdgeColor = FLinearColor(0.9f, 0.9f, 0.9f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Skill", meta = (ClampMin = "0"))
	int32 LevelNeedToLearn;

	UPROPERTY(EditDefaultsOnly, Category = "SkillTreeEdge")
	bool bShouldDrawTitle;

public:
	FLinearColor GetEdgeColor() { return EdgeColor; }

	virtual bool IsCanLearn(UAbilitySystemComponent* AS, int32 TargetLv, FText& CantReason);


};
