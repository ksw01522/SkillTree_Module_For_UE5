// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeEdge.h"

USkillTreeEdge::USkillTreeEdge()
{
    bShouldDrawTitle = true;
}

bool USkillTreeEdge::IsCanLearn(UAbilitySystemComponent* AS, int32 TargetLv, FText& CantReason)
{
    return false;
}
