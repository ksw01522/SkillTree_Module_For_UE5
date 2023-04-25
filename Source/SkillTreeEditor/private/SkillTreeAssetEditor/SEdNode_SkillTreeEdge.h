// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "SNodePanel.h"
#include "SGraphNode.h"

class STooltip;
class UEdGraphNode_SkillTreeEdge;

class SEdNode_SkillTreeEdge : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SEdNode_SkillTreeEdge) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode_SkillTreeEdge* InNode);

	virtual bool RequiresSecondPassLayout() const override;
	virtual void PerformSecondPassLayout(const TMap< UObject*, TSharedRef<SNode> >& NodeToWidgetLookup) const override;

	virtual void UpdateGraphNode() override;

	// Calculate position for multiple nodes to be placed between a start and end point, by providing this nodes index and max expected nodes 
	void PositionBetweenTwoNodesWithOffset(const FGeometry& StartGeom, const FGeometry& EndGeom, int32 NodeIndex, int32 MaxNodes) const;

	void OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo);

protected:
	FSlateColor GetEdgeColor() const;

	const FSlateBrush* GetEdgeImage() const;

	EVisibility GetEdgeImageVisibility() const;
	EVisibility GetEdgeTitleVisbility() const;
private:
	TSharedPtr<STextEntryPopup> TextEntryWidget;

};
