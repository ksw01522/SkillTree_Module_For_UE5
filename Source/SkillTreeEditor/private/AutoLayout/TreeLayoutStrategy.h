#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy.h"
#include "TreeLayoutStrategy.generated.h"

UCLASS()
class UTreeLayoutStrategy : public UAutoLayoutStrategy
{
	GENERATED_BODY()
public:
	UTreeLayoutStrategy();
	virtual ~UTreeLayoutStrategy();

	virtual void Layout(UEdGraph* EdGraph) override;

protected:
	void InitPass(USkillNode* RootNode, const FVector2D& Anchor);
	bool ResolveConflictPass(USkillNode* Node);

	bool ResolveConflict(USkillNode* LRoot, USkillNode* RRoot);

	void GetLeftContour(USkillNode* RootNode, int32 Level, TArray<UEdGraphNode_SkillNode*>& Contour);
	void GetRightContour(USkillNode* RootNode, int32 Level, TArray<UEdGraphNode_SkillNode*>& Contour);
	
	void ShiftSubTree(USkillNode* RootNode, const FVector2D& Offset);

	void UpdateParentNodePosition(USkillNode* RootNode);
};
