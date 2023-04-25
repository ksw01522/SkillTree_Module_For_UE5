// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "CustomLearnSkillConditionFactory.generated.h"

/**
 * 
 */
UCLASS()
class UCustomLearnSkillConditionFactory : public UFactory
{
	GENERATED_BODY()
public:
	UCustomLearnSkillConditionFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;

};
