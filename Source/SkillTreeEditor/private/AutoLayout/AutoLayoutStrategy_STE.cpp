#include "AutoLayout/AutoLayoutStrategy_STE.h"
#include "Kismet/KismetMathLibrary.h"
#include "SkillTreeAssetEditor/SEdNode_SkillNode.h"

UAutoLayoutStrategy_STE::UAutoLayoutStrategy_STE()
{
	Settings = nullptr;
	MaxIteration = 50;
	OptimalDistance = 150;
}

UAutoLayoutStrategy_STE::~UAutoLayoutStrategy_STE()
{

}

FBox2D UAutoLayoutStrategy_STE::GetNodeBound(UEdGraphNode* EdNode)
{
	int32 NodeWidth = GetNodeWidth(Cast<UEdGraphNode_SkillNode>(EdNode));
	int32 NodeHeight = GetNodeHeight(Cast<UEdGraphNode_SkillNode>(EdNode));
	FVector2D Min(EdNode->NodePosX, EdNode->NodePosY);
	FVector2D Max(EdNode->NodePosX + NodeWidth, EdNode->NodePosY + NodeHeight);
	return FBox2D(Min, Max);
}

FBox2D UAutoLayoutStrategy_STE::GetActualBounds(USkillNode* RootNode)
{
	int Level = 0;
	TArray<USkillNode*> CurrLevelNodes = { RootNode };
	TArray<USkillNode*> NextLevelNodes;

	FBox2D Rtn = GetNodeBound(EdGraph->NodeMap[RootNode]);

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			USkillNode* Node = CurrLevelNodes[i];
			check(Node != nullptr);

			Rtn += GetNodeBound(EdGraph->NodeMap[Node]);

			for (int j = 0; j < Node->ChildNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}
	return Rtn;
}

void UAutoLayoutStrategy_STE::RandomLayoutOneTree(USkillNode* RootNode, const FBox2D& Bound)
{
	int Level = 0;
	TArray<USkillNode*> CurrLevelNodes = { RootNode };
	TArray<USkillNode*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			USkillNode* Node = CurrLevelNodes[i];
			check(Node != nullptr);

			UEdGraphNode_SkillNode* EdNode_Node = EdGraph->NodeMap[Node];

			EdNode_Node->NodePosX = UKismetMathLibrary::RandomFloatInRange(Bound.Min.X, Bound.Max.X);
			EdNode_Node->NodePosY = UKismetMathLibrary::RandomFloatInRange(Bound.Min.Y, Bound.Max.Y);

			for (int j = 0; j < Node->ChildNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}
}

int32 UAutoLayoutStrategy_STE::GetNodeWidth(UEdGraphNode_SkillNode* EdNode)
{
	return EdNode->SEdNode->GetCachedGeometry().GetLocalSize().X;
}

int32 UAutoLayoutStrategy_STE::GetNodeHeight(UEdGraphNode_SkillNode* EdNode)
{
	return EdNode->SEdNode->GetCachedGeometry().GetLocalSize().Y;
}

