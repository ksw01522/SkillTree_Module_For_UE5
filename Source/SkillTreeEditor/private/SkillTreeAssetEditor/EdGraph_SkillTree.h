// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph_SkillTree.generated.h"

class USkillTree;
class USkillNode;
class USkillTreeEdge;
class UEdGraphNode_SkillNode;
class UEdGraphNode_SkillTreeEdge;

UCLASS()
class UEdGraph_SkillTree : public UEdGraph
{
	GENERATED_BODY()
	
public:
	UEdGraph_SkillTree();
	virtual ~UEdGraph_SkillTree();

	virtual void RebuildSkillTree();

	USkillTree* GetSkillTree() const;

	virtual bool Modify(bool bAlwaysMarkDirty = true) override;
	virtual void PostEditUndo() override;

	UPROPERTY(Transient)
	TMap<USkillNode*, UEdGraphNode_SkillNode*> NodeMap;

	UPROPERTY(Transient)
	TMap<USkillTreeEdge*, UEdGraphNode_SkillTreeEdge*> EdgeMap;

protected:
	void Clear();

	void SortNode(USkillNode* TargetNode);
};
