// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTree.h"
#include "SkillNode.h"

#define LOCTEXT_NAMESPACE "SkillTree"

USkillTree::USkillTree()
{
}

#if WITH_EDITOR
void USkillTree::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		USkillNode* Node = AllNodes[i];
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildNodes.Empty();
			Node->Edges.Empty();
		}
	}

	AllNodes.Empty();
	RootNodes.Empty();

}
#endif


#undef LOCTEXT_NAMESPACE