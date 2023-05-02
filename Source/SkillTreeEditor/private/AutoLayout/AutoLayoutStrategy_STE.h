#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "SkillTree.h"
#include "SkillTreeAssetEditor/EdGraph_SkillTree.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"
#include "SkillTreeAssetEditor/Settings_SkillTreeEditor.h"
#include "AutoLayoutStrategy_STE.generated.h"

UCLASS(abstract)
class UAutoLayoutStrategy_STE : public UObject
{
	GENERATED_BODY()
public:
	UAutoLayoutStrategy_STE();
	virtual ~UAutoLayoutStrategy_STE();

	virtual void Layout(UEdGraph* G) {};

	class USkillTreeEditorSettings* Settings;

protected:
	int32 GetNodeWidth(UEdGraphNode_SkillNode* EdNode);

	int32 GetNodeHeight(UEdGraphNode_SkillNode* EdNode);

	FBox2D GetNodeBound(UEdGraphNode* EdNode);

	FBox2D GetActualBounds(USkillNode* RootNode);

	virtual void RandomLayoutOneTree(USkillNode* RootNode, const FBox2D& Bound);

protected:
	USkillTree* Graph;
	UEdGraph_SkillTree* EdGraph;
	int32 MaxIteration;
	int32 OptimalDistance;
};
