// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "SkillNode.h"
#include "EdGraphNode_SkillNode.generated.h"

/**
 * 
 */
 class UEdGraphNode_SkillTreeEdge;
 class UEdGraph_SkillTree;
 class SEdNode_SkillNode;


UCLASS()
class UEdGraphNode_SkillNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UEdGraphNode_SkillNode();
	virtual ~UEdGraphNode_SkillNode();

	UPROPERTY(VisibleAnywhere, Instanced, Category = "SkillTree")
	USkillNode* SkillNode;

protected:
	void RegisterListeners();
	
	void CheckError();

public:
	void SetSkillNode(USkillNode* InNode);
	UEdGraph_SkillTree* GetSkillTreeEdGraph();

	SEdNode_SkillNode* SEdNode;

	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PrepareForCopying() override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;

	virtual FLinearColor GetBackgroundColor() const;
	virtual UEdGraphPin* GetInputPin() const;
	virtual UEdGraphPin* GetOutputPin() const;

	USkillNode* GetSkillNode() const;

	virtual void PostEditUndo() override;
	
	virtual void PostLoad() override;
	virtual void PostEditImport() override;
	virtual void PostPlacedNewNode() override;

	virtual void OnSkillNodePropertyChanged(const FPropertyChangedEvent& PropertyChangedEvent);

};
