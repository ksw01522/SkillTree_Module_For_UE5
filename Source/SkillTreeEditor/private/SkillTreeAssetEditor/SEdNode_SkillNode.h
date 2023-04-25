// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "SGraphPanel.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"



class SEdNode_SkillNode : public SGraphNode
{
protected:
	UEdGraphNode_SkillNode* EdSkillNode = nullptr;

	TSharedPtr<class STextBlock> DescriptionTextBlock = nullptr;

public:
	SLATE_BEGIN_ARGS(SEdNode_SkillNode){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode_SkillNode* InNode);

	virtual void UpdateGraphNode() override;

	virtual void CreatePinWidgets() override;
	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;
	virtual bool IsNameReadOnly() const override;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

	virtual FSlateColor GetBorderBackgroundColor() const;
	virtual FSlateColor GetBackgroundColor() const;

	virtual EVisibility GetDragOverMarkerVisibility() const;

	virtual const FSlateBrush* GetNameIcon() const;

	virtual	EVisibility GetDescriptionVisibility() const
	{
		// LOD this out once things get too small
		TSharedPtr<SGraphPanel> MyOwnerPanel = GetOwnerPanel();

		if (MyOwnerPanel.IsValid() && EdSkillNode != nullptr)// && EdSkillNode->CanVisibleDescription())
		{
			return MyOwnerPanel->GetCurrentLOD() > EGraphRenderingLOD::LowDetail ? EVisibility::Visible : EVisibility::Collapsed;
		}
		else
		{
			return EVisibility::Collapsed;
		}
	}

	virtual FText GetDescription() const;

	void CheckError();

protected:
	TSharedPtr<class SCommentBubble> CreateCommentBubble();
	virtual TSharedPtr<SCompoundWidget> CreateNodeBody();

};

