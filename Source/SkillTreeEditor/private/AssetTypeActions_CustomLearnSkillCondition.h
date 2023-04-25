// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

/**
 * 
 */
class FAssetTypeActions_CustomLearnSkillCondition : public FAssetTypeActions_Base
{

public:
	FAssetTypeActions_CustomLearnSkillCondition(EAssetTypeCategories::Type InAssetCategory) : AssetCategory(InAssetCategory) {}

	// Same Color as the Blueprints
	virtual FColor GetTypeColor() const override { return FColor(63, 126, 255); }
	virtual uint32 GetCategories() override { return AssetCategory; }
	virtual bool HasActions(const TArray<UObject*>& InObjects) const override { return false; }

	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;



protected:
	EAssetTypeCategories::Type AssetCategory;
};
