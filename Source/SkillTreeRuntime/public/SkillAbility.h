// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SkillAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SKILLTREERUNTIME_API USkillAbility : public UGameplayAbility
{
	GENERATED_BODY()

	USkillAbility();
	virtual ~USkillAbility(){}

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter = GetSkillName, meta = ( AllowPrivateAcess = "true"))
	FText SkillName;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintGetter, Category = "SkillAbility")
	FText GetSkillName() { return SkillName; }

	UFUNCTION(BlueprintNativeEvent)
	FText GetSkillDescription();
	FText GetSkillDescription_Implementation();

};
