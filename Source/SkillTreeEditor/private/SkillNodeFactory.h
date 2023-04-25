// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <EdGraphUtilities.h>
#include <EdGraph/EdGraphNode.h>

/**
 * 
 */
class FGraphPanelNodeFactory_SkillTree : public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override;

};
