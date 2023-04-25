// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillNodeFactory.h"
#include <EdGraph/EdGraphNode.h>
#include "SkillTreeAssetEditor/SEdNode_SkillTreeEdge.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/SEdNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"



TSharedPtr<class SGraphNode> FGraphPanelNodeFactory_SkillTree::CreateNode(UEdGraphNode* Node) const
{
	if (UEdGraphNode_SkillNode* EdNode_GraphNode = Cast<UEdGraphNode_SkillNode>(Node))
	{
		return SNew(SEdNode_SkillNode, EdNode_GraphNode);
	}
	else if (UEdGraphNode_SkillTreeEdge* EdNode_Edge = Cast<UEdGraphNode_SkillTreeEdge>(Node))
	{
		return SNew(SEdNode_SkillTreeEdge, EdNode_Edge);
	}
	return nullptr;
}
