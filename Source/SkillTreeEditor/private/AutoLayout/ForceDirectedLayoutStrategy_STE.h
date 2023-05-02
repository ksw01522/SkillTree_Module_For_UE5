#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy_STE.h"
#include "ForceDirectedLayoutStrategy_STE.generated.h"

UCLASS()
class UForceDirectedLayoutStrategy_STE : public UAutoLayoutStrategy_STE
{
	GENERATED_BODY()
public:
	UForceDirectedLayoutStrategy_STE();
	virtual ~UForceDirectedLayoutStrategy_STE();

	virtual void Layout(UEdGraph* EdGraph) override;

protected:
	virtual FBox2D LayoutOneTree(USkillNode* RootNode, const FBox2D& PreTreeBound);

protected:
	bool bRandomInit;
	float InitTemperature;
	float CoolDownRate;
};
