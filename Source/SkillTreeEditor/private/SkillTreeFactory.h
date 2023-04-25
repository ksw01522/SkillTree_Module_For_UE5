// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "SkillTree.h"
#include "SkillTreeFactory.generated.h"

/**
 * 
 */
UCLASS()
class USkillTreeFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	USkillTreeFactory();

	//UPROPERTY(EditAnywhere, Category = DataAsset)
	//TSubclassOf<UGenericGraph> SkillTreeClass;

	//bool ConfigureProperties();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual FText GetDisplayName() const override;
	virtual FString GetDefaultNewAssetName() const override;
};
