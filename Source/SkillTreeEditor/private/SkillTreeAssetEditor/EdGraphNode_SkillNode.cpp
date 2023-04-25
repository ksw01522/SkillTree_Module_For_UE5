// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraph_SkillTree.h"
#include "Kismet2/Kismet2NameValidators.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "EdGraphNode_SkillNode"

UEdGraphNode_SkillNode::UEdGraphNode_SkillNode()
{
	bCanRenameNode = true;
}

UEdGraphNode_SkillNode::~UEdGraphNode_SkillNode()
{

}


UEdGraph_SkillTree* UEdGraphNode_SkillNode::GetSkillTreeEdGraph()
{
	return Cast<UEdGraph_SkillTree>(GetGraph());
}

void UEdGraphNode_SkillNode::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, "MultipleNodes", FName(), TEXT("In"));
	CreatePin(EGPD_Output, "MultipleNodes", FName(), TEXT("Out"));
}

FText UEdGraphNode_SkillNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (SkillNode == nullptr)
	{
		return Super::GetNodeTitle(TitleType);
	}
	else
	{
		return SkillNode->GetNodeTitle();
	}
}



void UEdGraphNode_SkillNode::PrepareForCopying()
{
	SkillNode->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
}

void UEdGraphNode_SkillNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);

	if (FromPin != nullptr)
	{
		if (GetSchema()->TryCreateConnection(FromPin, GetInputPin()))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}

FLinearColor UEdGraphNode_SkillNode::GetBackgroundColor() const
{
	return SkillNode == nullptr ? FLinearColor::Black : SkillNode->GetBackgroundColor();
}

UEdGraphPin* UEdGraphNode_SkillNode::GetInputPin() const
{
	return Pins[0];
}

UEdGraphPin* UEdGraphNode_SkillNode::GetOutputPin() const
{
	return Pins[1];
}

USkillNode* UEdGraphNode_SkillNode::GetSkillNode() const
{
	return SkillNode;
}

void UEdGraphNode_SkillNode::SetSkillNode(USkillNode* InNode)
{
	SkillNode = InNode;
}

#if WITH_EDITOR

void UEdGraphNode_SkillNode::PostEditUndo()
{
	UEdGraphNode::PostEditUndo();
}

#endif

#undef LOCTEXT_NAMSPACE