#pragma once

#include "CoreMinimal.h"
#include "AutoLayoutStrategy_STE.h"
#include "TreeLayoutStrategy_STE.generated.h"

UCLASS()
class UTreeLayoutStrategy_STE : public UAutoLayoutStrategy_STE
{
	GENERATED_BODY()
public:
	UTreeLayoutStrategy_STE();
	virtual ~UTreeLayoutStrategy_STE();

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
