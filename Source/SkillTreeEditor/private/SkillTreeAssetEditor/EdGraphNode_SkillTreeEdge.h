// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "SkillTreeEdge.h"
#include "EdGraphNode_SkillTreeEdge.generated.h"

class USkillNode;
class UEdGraphNode_SkillNode;

/**
 * 
 */
UCLASS(MinimalAPI)
class UEdGraphNode_SkillTreeEdge : public UEdGraphNode
{
	GENERATED_BODY()
	
public:
	UEdGraphNode_SkillTreeEdge();

	UPROPERTY()
	class UEdGraph* Graph;

	UPROPERTY(VisibleAnywhere, Instanced, Category = "SkillTree")
	USkillTreeEdge* SkillTreeEdge;

	void SetEdge(USkillTreeEdge* Edge);

	virtual void AllocateDefaultPins();

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;

	virtual void PrepareForCopying() override;

	virtual UEdGraphPin* GetInputPin() const {return Pins[0]; }
	virtual UEdGraphPin* GetOutputPin() const {return Pins[1]; }

	void CreateConnections(UEdGraphNode_SkillNode* Start, UEdGraphNode_SkillNode* End);

	UEdGraphNode_SkillNode* GetStartNode();
	UEdGraphNode_SkillNode* GetEndNode();
};
