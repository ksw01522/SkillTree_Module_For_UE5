// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillNode.h"

#define LOCTEXT_NAMESPACE "EdGraphNode_SkillTreeEdge"

UEdGraphNode_SkillTreeEdge::UEdGraphNode_SkillTreeEdge()
{
	bCanRenameNode = true;
}

void UEdGraphNode_SkillTreeEdge::SetEdge(USkillTreeEdge* Edge)
{
	SkillTreeEdge = Edge;
}

void UEdGraphNode_SkillTreeEdge::AllocateDefaultPins()
{
	UEdGraphPin* Inputs = CreatePin(EGPD_Input, TEXT("Edge"), FName(), TEXT("In"));
	Inputs->bHidden = true;
	UEdGraphPin* Outputs = CreatePin(EGPD_Output, TEXT("Edge"), FName(), TEXT("Out"));
	Outputs->bHidden = true;
}

FText UEdGraphNode_SkillTreeEdge::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::GetEmpty();
	/*
	if (SkillTreeEdge)
	{
		return SkillTreeEdge->GetNodeTitle();
	}

	return FText();
	*/
}

void UEdGraphNode_SkillTreeEdge::PinConnectionListChanged(UEdGraphPin* Pin)
{
	if (Pin->LinkedTo.Num() == 0)
	{
		Modify();

		if (UEdGraph* ParentGraph = GetGraph())
		{
			ParentGraph->Modify();
		}

		DestroyNode();
	}
}

void UEdGraphNode_SkillTreeEdge::PrepareForCopying()
{
	SkillTreeEdge->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdGraphNode_SkillTreeEdge::CreateConnections(UEdGraphNode_SkillNode* Start, UEdGraphNode_SkillNode* End)
{
	Pins[0]->Modify();
	Pins[0]->LinkedTo.Empty();

	Start->GetOutputPin()->Modify();
	Pins[0]->MakeLinkTo(Start->GetOutputPin());

	// This to next
	Pins[1]->Modify();
	Pins[1]->LinkedTo.Empty();

	End->GetInputPin()->Modify();
	Pins[1]->MakeLinkTo(End->GetInputPin());
}

UEdGraphNode_SkillNode* UEdGraphNode_SkillTreeEdge::GetStartNode()
{
	if (0 < Pins[0]->LinkedTo.Num())
	{
		return Cast<UEdGraphNode_SkillNode>(Pins[0]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

UEdGraphNode_SkillNode* UEdGraphNode_SkillTreeEdge::GetEndNode()
{
	if (0 < Pins[1]->LinkedTo.Num())
	{
		return Cast<UEdGraphNode_SkillNode>(Pins[1]->LinkedTo[0]->GetOwningNode());
	}
	else
	{
		return nullptr;
	}
}

#undef LOCTEXT_NAMESPACE