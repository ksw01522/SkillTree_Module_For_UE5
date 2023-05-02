#include "AutoLayout/TreeLayoutStrategy_STE.h"
#include "SkillTreeEditor.h"
#include "SkillTreeAssetEditor/SEdNode_SkillNode.h"

UTreeLayoutStrategy_STE::UTreeLayoutStrategy_STE()
{
}

UTreeLayoutStrategy_STE::~UTreeLayoutStrategy_STE()
{

}

void UTreeLayoutStrategy_STE::Layout(UEdGraph* _EdGraph)
{
	EdGraph = Cast<UEdGraph_SkillTree>(_EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildSkillTree();
	Graph = EdGraph->GetSkillTree();
	check(Graph != nullptr);

	bool bFirstPassOnly = false;

	if (Settings != nullptr)
	{
		OptimalDistance = Settings->OptimalDistance;
		MaxIteration = Settings->MaxIteration;
		bFirstPassOnly = Settings->bFirstPassOnly;
	}

	FVector2D Anchor(0.f, 0.f);
	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		USkillNode* RootNode = Graph->RootNodes[i];
		InitPass(RootNode, Anchor);

		if (!bFirstPassOnly)
		{
			for (int32 j = 0; j < MaxIteration; ++j)
			{
				bool HasConflict = ResolveConflictPass(RootNode);
				if (!HasConflict)
				{
					break;
				}
			}
		}
	}

	for (int32 i = 0; i < Graph->RootNodes.Num(); ++i)
	{
		for (int32 j = 0; j < i; ++j)
		{
			ResolveConflict(Graph->RootNodes[j], Graph->RootNodes[i]);
		}
	}
}

void UTreeLayoutStrategy_STE::InitPass(USkillNode* RootNode, const FVector2D& Anchor)
{
	UEdGraphNode_SkillNode* EdNode_RootNode = EdGraph->NodeMap[RootNode];

	FVector2D ChildAnchor(FVector2D(0.f, GetNodeHeight(EdNode_RootNode) + OptimalDistance + Anchor.Y));
	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		USkillNode* Child = RootNode->ChildNodes[i];
		UEdGraphNode_SkillNode* EdNode_ChildNode = EdGraph->NodeMap[Child];
		if (i > 0)
		{
			USkillNode* PreChild = RootNode->ChildNodes[i - 1];
			UEdGraphNode_SkillNode* EdNode_PreChildNode = EdGraph->NodeMap[PreChild];
			ChildAnchor.X += OptimalDistance + GetNodeWidth(EdNode_PreChildNode) / 2;
		}
		ChildAnchor.X += GetNodeWidth(EdNode_ChildNode) / 2;
		InitPass(Child, ChildAnchor);
	}
	
	float NodeWidth = GetNodeWidth(EdNode_RootNode);

	EdNode_RootNode->NodePosY = Anchor.Y;
	if (RootNode->ChildNodes.Num() == 0)
	{
		EdNode_RootNode->NodePosX = Anchor.X - NodeWidth / 2;
	}
	else
	{
		UpdateParentNodePosition(RootNode);
	}
}

bool UTreeLayoutStrategy_STE::ResolveConflictPass(USkillNode* Node)
{
	bool HasConflict = false;
	for (int32 i = 0; i < Node->ChildNodes.Num(); ++i)
	{
		USkillNode* Child = Node->ChildNodes[i];
		if (ResolveConflictPass(Child))
		{
			HasConflict = true;
		}
	}

	for (int32 i = 0; i < Node->ParentNodes.Num(); ++i)
	{
		USkillNode* ParentNode = Node->ParentNodes[i];
		for (int32 j = 0; j < ParentNode->ChildNodes.Num(); ++j)
		{
			USkillNode* LeftSibling = ParentNode->ChildNodes[j];
			if (LeftSibling == Node)
				break;
			if (ResolveConflict(LeftSibling, Node))
			{
				HasConflict = true;
			}
		}
	}

	return HasConflict;
}

bool UTreeLayoutStrategy_STE::ResolveConflict(USkillNode* LRoot, USkillNode* RRoot)
{
	TArray<UEdGraphNode_SkillNode*> RightContour, LeftContour;

	GetRightContour(LRoot, 0, RightContour);
	GetLeftContour(RRoot, 0, LeftContour);

	int32 MaxOverlapDistance = 0;
	int32 Num = FMath::Min(LeftContour.Num(), RightContour.Num());
	for (int32 i = 0; i < Num; ++i)
	{
		if (RightContour.Contains(LeftContour[i]) || LeftContour.Contains(RightContour[i]))
			break;

		int32 RightBound = RightContour[i]->NodePosX + GetNodeWidth(RightContour[i]);
		int32 LeftBound = LeftContour[i]->NodePosX;
		int32 Distance = RightBound + OptimalDistance - LeftBound;
		if (Distance > MaxOverlapDistance)
		{
			MaxOverlapDistance = Distance;
		}
	}

	if (MaxOverlapDistance > 0)
	{
		ShiftSubTree(RRoot, FVector2D(MaxOverlapDistance, 0.f));

		TArray<USkillNode*> ParentNodes = RRoot->ParentNodes;
		TArray<USkillNode*> NextParentNodes;
		while (ParentNodes.Num() != 0)
		{
			for (int32 i = 0; i < ParentNodes.Num(); ++i)
			{
				UpdateParentNodePosition(ParentNodes[i]);

				NextParentNodes.Append(ParentNodes[i]->ParentNodes);
			}

			ParentNodes = NextParentNodes;
			NextParentNodes.Reset();
		}

		return true;
	}
	else
	{
		return false;
	}
}

void UTreeLayoutStrategy_STE::GetLeftContour(USkillNode* RootNode, int32 Level, TArray<UEdGraphNode_SkillNode*>& Contour)
{
	UEdGraphNode_SkillNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX < Contour[Level]->NodePosX)
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		GetLeftContour(RootNode->ChildNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy_STE::GetRightContour(USkillNode* RootNode, int32 Level, TArray<UEdGraphNode_SkillNode*>& Contour)
{
	UEdGraphNode_SkillNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	if (Level >= Contour.Num())
	{
		Contour.Add(EdNode_Node);
	}
	else if (EdNode_Node->NodePosX + GetNodeWidth(EdNode_Node) > Contour[Level]->NodePosX + GetNodeWidth(Contour[Level]))
	{
		Contour[Level] = EdNode_Node;
	}

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		GetRightContour(RootNode->ChildNodes[i], Level + 1, Contour);
	}
}

void UTreeLayoutStrategy_STE::ShiftSubTree(USkillNode* RootNode, const FVector2D& Offset)
{
	UEdGraphNode_SkillNode* EdNode_Node = EdGraph->NodeMap[RootNode];
	EdNode_Node->NodePosX += Offset.X;
	EdNode_Node->NodePosY += Offset.Y;

	for (int32 i = 0; i < RootNode->ChildNodes.Num(); ++i)
	{
		USkillNode* Child = RootNode->ChildNodes[i];

		if (Child->ParentNodes[0] == RootNode)
		{
			ShiftSubTree(RootNode->ChildNodes[i], Offset);
		}
	}
}

void UTreeLayoutStrategy_STE::UpdateParentNodePosition(USkillNode* ParentNode)
{
	UEdGraphNode_SkillNode* EdNode_ParentNode = EdGraph->NodeMap[ParentNode];
	if (ParentNode->ChildNodes.Num() % 2 == 0)
	{
		UEdGraphNode_SkillNode* FirstChild = EdGraph->NodeMap[ParentNode->ChildNodes[0]];
		UEdGraphNode_SkillNode* LastChild = EdGraph->NodeMap[ParentNode->ChildNodes.Last()];
		float LeftBound = FirstChild->NodePosX;
		float RightBound = LastChild->NodePosX + GetNodeWidth(LastChild);
		EdNode_ParentNode->NodePosX = (LeftBound + RightBound) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
	else
	{
		UEdGraphNode_SkillNode* MidChild = EdGraph->NodeMap[ParentNode->ChildNodes[ParentNode->ChildNodes.Num() / 2]];
		EdNode_ParentNode->NodePosX = MidChild->NodePosX + GetNodeWidth(MidChild) / 2 - GetNodeWidth(EdNode_ParentNode) / 2;
	}
}
