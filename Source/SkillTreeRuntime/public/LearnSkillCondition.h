// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LearnSkillCondition.generated.h"

/**
 * 
 */
class UAbilitySystemComponent;

UCLASS(Abstract, EditInlineNew, Blueprintable, BlueprintType)
class SKILLTREERUNTIME_API ULearnSkillCondition : public UObject
{
	GENERATED_BODY()
	
public:
	ULearnSkillCondition();
	virtual ~ULearnSkillCondition() {}

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, BlueprintCallable, Category = "Skill|Learn")
	bool IsCanLearn(UAbilitySystemComponent* AS, int32 TargetLv, FText& CantReason);
	virtual bool IsCanLearn_Implementation(UAbilitySystemComponent* AS, int32 TargetLv, FText& CantReason);

};
