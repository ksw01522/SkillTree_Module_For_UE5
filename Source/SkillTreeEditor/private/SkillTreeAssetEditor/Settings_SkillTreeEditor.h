#pragma once

#include "CoreMinimal.h"
#include "Settings_SkillTreeEditor.generated.h"

UENUM(BlueprintType)
enum class EAutoLayoutStrategy_STE : uint8
{
	Tree,
	ForceDirected,
};

UCLASS(Config = DefaultEditor)
class USkillTreeEditorSettings : public UObject
{
	GENERATED_BODY()

public:
	USkillTreeEditorSettings();
	virtual ~USkillTreeEditorSettings();

	UPROPERTY(EditDefaultsOnly, Category = "AutoArrange")
	float OptimalDistance;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	EAutoLayoutStrategy_STE AutoLayoutStrategy;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	int32 MaxIteration;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bFirstPassOnly;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	bool bRandomInit;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float InitTemperature;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "AutoArrange")
	float CoolDownRate;


};
