// Fill out your copyright notice in the Description page of Project Settings.


#include "LearnSkillCondition.h"
#include "AbilitySystemComponent.h"

#define LOCTEXT_NAMESPACE "Learn Skill Condition"

ULearnSkillCondition::ULearnSkillCondition()
{
	
}

bool ULearnSkillCondition::IsCanLearn_Implementation(UAbilitySystemComponent* AS, int32 TargetLv, FText& CantReason)
{
	if(!IsValid(AS)){
		CantReason = LOCTEXT("Reason for Can't Find Ability System", "스킬 시스템을 찾을 수 없습니다");
		return false;
	}


	return true;
}


#undef LOCTEXT_NAMESPACE