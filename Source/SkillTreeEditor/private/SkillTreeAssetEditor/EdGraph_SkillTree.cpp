// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/EdGraph_SkillTree.h"
#include "SkillTreeEditor.h"
#include "SkillTree.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"

UEdGraph_SkillTree::UEdGraph_SkillTree()
{
}


UEdGraph_SkillTree::~UEdGraph_SkillTree()
{
}

void UEdGraph_SkillTree::RebuildSkillTree()
{
	LOG_INFO(TEXT("has been called"));

	USkillTree* SkillTree = GetSkillTree();

	Clear();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		//노드 리빌드 작업
		if (UEdGraphNode_SkillNode* EdNode = Cast<UEdGraphNode_SkillNode>(Nodes[i]))
		{
			if (EdNode->SkillNode == nullptr) continue;

			USkillNode* CustomNode = EdNode->SkillNode;

			NodeMap.Add(CustomNode, EdNode);

			SkillTree->AllNodes.Add(CustomNode);

			for (int PinIdx = 0; PinIdx < EdNode->Pins.Num(); ++PinIdx)
			{
				UEdGraphPin* Pin = EdNode->Pins[PinIdx];
				if(Pin->Direction != EEdGraphPinDirection::EGPD_Output) continue;	//출력핀이 아니면 다음으로

				for (int LinkToIdx = 0; LinkToIdx < Pin->LinkedTo.Num(); ++LinkToIdx)
				{
					USkillNode* ChildNode = nullptr;

					//핀이 노드랑 연결되어 있으면
					if (UEdGraphNode_SkillNode* EdNode_Child = Cast<UEdGraphNode_SkillNode>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						ChildNode = EdNode_Child->SkillNode;
					}
					//노드랑 연결되어있는 것이 Edge라 Edge를 통해 자식 노드를 탐색
					else if(UEdGraphNode_SkillTreeEdge* EdNode_Edge = Cast<UEdGraphNode_SkillTreeEdge>(Pin->LinkedTo[LinkToIdx]->GetOwningNode()))
					{
						UEdGraphNode_SkillNode* ChildEdNodeFromEdge = EdNode_Edge->GetEndNode();
						if (ChildEdNodeFromEdge != nullptr)
						{
							ChildNode = ChildEdNodeFromEdge->SkillNode;
						}
					}

					//탐색해서 찾아냈다면
					if (ChildNode != nullptr)
					{
						CustomNode->ChildNodes.Add(ChildNode);
						ChildNode->ParentNodes.Add(CustomNode);
					}
					else
					{
						LOG_ERROR(TEXT("Can't find Child Node"));
					}
				}
			}
		}
		//간선 리빌드 작업
		else if (UEdGraphNode_SkillTreeEdge* EdgeNode = Cast<UEdGraphNode_SkillTreeEdge>(Nodes[i]))
		{
			UEdGraphNode_SkillNode* StartNode = EdgeNode->GetStartNode();
			UEdGraphNode_SkillNode* EndNode = EdgeNode->GetEndNode();
			USkillTreeEdge* Edge = EdgeNode->SkillTreeEdge;

			if (StartNode == nullptr || EndNode == nullptr || Edge == nullptr)
			{
				LOG_ERROR(TEXT("Add edge failed."));
				continue;
			}

			EdgeMap.Add(Edge, EdgeNode);

			Edge->InTree = SkillTree;
			Edge->Rename(nullptr, SkillTree, REN_DontCreateRedirectors | REN_DoNotDirty);
			Edge->StartNode = StartNode->SkillNode;
			Edge->EndNode = EndNode->SkillNode;
			Edge->StartNode->Edges.Add(Edge->EndNode, Edge);
		}
	}

	for (int i = 0; i < SkillTree->AllNodes.Num(); ++i)
	{
		USkillNode* Node = SkillTree->AllNodes[i];
		SortNode(Node);
		
		if (Node->ParentNodes.Num() == 0)
		{
			SkillTree->RootNodes.Add(Node);
		}
		

		Node->InTree = SkillTree;
		Node->Rename(nullptr, SkillTree, REN_DontCreateRedirectors | REN_DoNotDirty);

		Node->RebuildNode();
	}

	
	SkillTree->RootNodes.Sort([&](const USkillNode& L, const USkillNode& R)
	{
		UEdGraphNode_SkillNode* EdNode_LNode = NodeMap[&L];
		UEdGraphNode_SkillNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	});
	

}

USkillTree* UEdGraph_SkillTree::GetSkillTree() const
{
	return CastChecked<USkillTree>(GetOuter());
}

bool UEdGraph_SkillTree::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool FinalValue = Super::Modify(bAlwaysMarkDirty);

	GetSkillTree()->Modify();

	for (int32 i = 0; i <  Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return FinalValue;
}

void UEdGraph_SkillTree::Clear()
{
	USkillTree* SkillTree = GetSkillTree();

	SkillTree->ClearGraph();
	NodeMap.Reset();
	EdgeMap.Reset();

	for (int i = 0; i < Nodes.Num(); ++i)
	{
		if (UEdGraphNode_SkillNode* EdNode = Cast<UEdGraphNode_SkillNode>(Nodes[i]))
		{
			USkillNode* CustomNode = EdNode->SkillNode;
			if (CustomNode)
			{
				CustomNode->ParentNodes.Reset();
				CustomNode->ChildNodes.Reset();
				CustomNode->Edges.Reset();
			}
		}
	}
}

void UEdGraph_SkillTree::SortNode(USkillNode* TargetNode)
{
	USkillNode* Node = TargetNode;

	auto Comp = [&](const USkillNode& L, const USkillNode& R)
	{
		UEdGraphNode_SkillNode* EdNode_LNode = NodeMap[&L];
		UEdGraphNode_SkillNode* EdNode_RNode = NodeMap[&R];
		return EdNode_LNode->NodePosX < EdNode_RNode->NodePosX;
	};

	Node->ChildNodes.Sort(Comp);
	Node->ParentNodes.Sort(Comp);
}

void UEdGraph_SkillTree::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}