// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/SEdNode_SkillNode.h"
#include "SkillTreeEditor.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/Colors_SkillTree.h"
#include "SLevelOfDetailBranchNode.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "SCommentBubble.h"
#include "SlateOptMacros.h"
#include "SGraphPin.h"
#include "GraphEditorSettings.h"
#include "SkillTreeAssetEditor/SkillNodeDragConnection.h"
#include "SkillTree.h"
#include "Styling/AppStyle.h"


#define LOCTEXT_NAMESPACE "EdGraphNode_SkillNode"

class SCustomGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SCustomGraphPin) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin)
	{
		this->SetCursor(EMouseCursor::Default);

		bShowLabel = true;

		GraphPinObj = InPin;
		check(GraphPinObj != nullptr);

		const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
		check(Schema);

		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SCustomGraphPin::GetPinBorder)
			.BorderBackgroundColor(this, &SCustomGraphPin::GetPinColor)
			.OnMouseButtonDown(this, &SCustomGraphPin::OnPinMouseDown)
			.Cursor(this, &SCustomGraphPin::GetPinCursor)
			.Padding(FMargin(5.0f))
		);
	}

protected:
	virtual FSlateColor GetPinColor() const override
	{
		return SkillTreeColors::Pin::Default;
	}

	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override
	{
		return SNew(STextBlock);
	}

	const FSlateBrush* GetPinBorder() const
	{
		return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
	}

	virtual TSharedRef<FDragDropOperation> SpawnPinDragEvent(const TSharedRef<class SGraphPanel>& InGraphPanel, const TArray< TSharedRef<SGraphPin> >& InStartingPins) override
	{
		FSkillNodeDragConnection::FDraggedPinTable PinHandles;
		PinHandles.Reserve(InStartingPins.Num());
		// since the graph can be refreshed and pins can be reconstructed/replaced 
		// behind the scenes, the DragDropOperation holds onto FGraphPinHandles 
		// instead of direct widgets/graph-pins
		for (const TSharedRef<SGraphPin>& PinWidget : InStartingPins)
		{
			PinHandles.Add(PinWidget->GetPinObj());
		}

		return FSkillNodeDragConnection::New(InGraphPanel, PinHandles);
	}

};

void SEdNode_SkillNode::Construct(const FArguments& InArgs, UEdGraphNode_SkillNode* InNode)
{
	EdSkillNode = InNode;
	GraphNode = InNode;
	UpdateGraphNode();
	InNode->SEdNode = this;
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SEdNode_SkillNode::UpdateGraphNode()
{
	const FMargin NodePadding = FMargin(5);
	const FMargin NamePadding = FMargin(2);

	InputPins.Empty();
	OutputPins.Empty();

	// Reset variables that are going to be exposed, in case we are refreshing an already setup node.
	RightNodeBox.Reset();
	LeftNodeBox.Reset();

	ErrorColor = SkillTreeColors::NodeBody::Error;

	TSharedPtr<SCompoundWidget> NodeBody = CreateNodeBody().ToSharedRef();

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
			.Padding(0.0f)
			.BorderBackgroundColor(this, &SEdNode_SkillNode::GetBorderBackgroundColor)
			[
				SNew(SOverlay)
					+ SOverlay::Slot()
					.HAlign(HAlign_Fill)
					.VAlign(VAlign_Fill)
					[
						SNew(SVerticalBox)

							// Input Pin Area
							+ SVerticalBox::Slot()
							.FillHeight(1)
							[
								SAssignNew(LeftNodeBox, SVerticalBox)
							]

						// Output Pin Area	
							+ SVerticalBox::Slot()
							.FillHeight(1)
							[
								SAssignNew(RightNodeBox, SVerticalBox)
							]
					]

					+ SOverlay::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Padding(8.0f)
					[
						NodeBody.ToSharedRef()
					]
			]
		];


	// Create comment bubble
	CreateCommentBubble();
	
	CreatePinWidgets();

	CheckError();
}


void SEdNode_SkillNode::CreatePinWidgets()
{
	UEdGraphNode_SkillNode* StateNode = EdSkillNode;

	for (int32 PinIdx = 0; PinIdx < StateNode->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = StateNode->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SCustomGraphPin, MyPin);

			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SEdNode_SkillNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility(TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced));
	}

	TSharedPtr<SVerticalBox> PinBox;
	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		PinBox = LeftNodeBox;
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		PinBox = RightNodeBox;
		OutputPins.Add(PinToAdd);
	}

	if (PinBox)
	{
		PinBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			//.Padding(6.0f, 0.0f)
			[
				PinToAdd
			];
	}
}

bool SEdNode_SkillNode::IsNameReadOnly() const
{
	UEdGraphNode_SkillNode* EdNode_Node = EdSkillNode;
	check(EdNode_Node != nullptr);

	USkillTree* SkillTree = EdNode_Node->SkillNode->InTree;
	check(SkillTree != nullptr);

	return true;
}

FText SEdNode_SkillNode::GetDescription() const
{
	return FText::GetEmpty();
}

void SEdNode_SkillNode::CheckError()
{
	SetupErrorReporting();
}

TSharedPtr<SCommentBubble> SEdNode_SkillNode::CreateCommentBubble()
{
	TSharedPtr<SCommentBubble> CommentBubble;
	const FSlateColor CommentColor = GetDefault<UGraphEditorSettings>()->DefaultCommentNodeTitleColor;
	
	SAssignNew(CommentBubble, SCommentBubble)
		.GraphNode(EdSkillNode)
		.Text(this, &SGraphNode::GetNodeComment)
		.OnTextCommitted(this, &SGraphNode::OnCommentTextCommitted)
		.ColorAndOpacity(CommentColor)
		.AllowPinning(true)
		.EnableTitleBarBubble(true)
		.EnableBubbleCtrls(true)
		.GraphLOD(this, &SGraphNode::GetCurrentLOD)
		.IsGraphNodeHovered(this, &SGraphNode::IsHovered);

	GetOrAddSlot(ENodeZone::TopCenter)
		.SlotOffset(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetOffset))
		.SlotSize(TAttribute<FVector2D>(CommentBubble.Get(), &SCommentBubble::GetSize))
		.AllowScaling(TAttribute<bool>(CommentBubble.Get(), &SCommentBubble::IsScalingAllowed))
		.VAlign(VAlign_Top)
		[
			CommentBubble.ToSharedRef()
		];

	return CommentBubble;
}

TSharedPtr<SCompoundWidget> SEdNode_SkillNode::CreateNodeBody()
{
	TSharedPtr<SBorder> NodeBody;

	FLinearColor TitleShadowColor(0.6f, 0.6f, 0.6f);
	const FSlateBrush* NodeTypeIcon = GetNameIcon();
	TSharedPtr<SNodeTitle> NodeTitle = SNew(SNodeTitle, EdSkillNode);
	
	//Create Body
	SAssignNew(NodeBody, SBorder)
	.BorderImage(FAppStyle::GetBrush("Graph.StateNode.ColorSpill"))
	.BorderBackgroundColor(TitleShadowColor)
	.HAlign(HAlign_Center)
	.VAlign(VAlign_Center)
	.Visibility(EVisibility::SelfHitTestInvisible)
	.Padding(6.0f)
	[
		SNew(SVerticalBox)
		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			// Error message
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ErrorReporting, SErrorText)
				.BackgroundColor(this, &SEdNode_SkillNode::GetErrorColor)
				.ToolTipText(this, &SEdNode_SkillNode::GetErrorMsgToolTip)
			]
				// Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SImage)
				.Image(NodeTypeIcon)
			]

			// Body
			+ SHorizontalBox::Slot()
			.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
			[
				SNew(SVerticalBox)
				
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(InlineEditableText, SInlineEditableTextBlock)
					.Style(FAppStyle::Get(), "Graph.StateNode.NodeTitleInlineEditableText")
					.Text(NodeTitle.Get(), &SNodeTitle::GetHeadTitle)
					.OnVerifyTextChanged(this, &SEdNode_SkillNode::OnVerifyNameTextChanged)
					.OnTextCommitted(this, &SEdNode_SkillNode::OnNameTextCommited)
					.IsReadOnly(this, &SEdNode_SkillNode::IsNameReadOnly)
					.IsSelected(this, &SEdNode_SkillNode::IsSelectedExclusively)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					NodeTitle.ToSharedRef()
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(8.0f, 0.0f, 8.0f, 0.0f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.AutoHeight()
				[
					SAssignNew(DescriptionTextBlock, STextBlock)
					.Visibility(this, &SEdNode_SkillNode::GetDescriptionVisibility)
					.Text(this, &SEdNode_SkillNode::GetDescription)
					//.WrapTextAt(5)
					.Margin(FMargin(6.0f, 4.0f, 6.0f, 1.0f))
				]
			]
		]
	];


	return NodeBody;
}



END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SEdNode_SkillNode::OnNameTextCommited(const FText& InText, ETextCommit::Type CommitInfo)
{
	SGraphNode::OnNameTextCommited(InText, CommitInfo);

	UEdGraphNode_SkillNode* MyNode = EdSkillNode;

	if (MyNode != nullptr && MyNode->SkillNode != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("Skill Tree Editor Rename Node", "Skill Tree Editor: Rename Node"));
		MyNode->Modify();
		MyNode->SkillNode->Modify();
		//MyNode->SkillNode->SetNodeTitle(InText);
		UpdateGraphNode();
	}
}

FSlateColor SEdNode_SkillNode::GetBorderBackgroundColor() const
{
	UEdGraphNode_SkillNode* MyNode = EdSkillNode;
	return MyNode ? MyNode->GetBackgroundColor() : SkillTreeColors::NodeBorder::HighlightAbortRange0;
}

FSlateColor SEdNode_SkillNode::GetBackgroundColor() const
{
	return SkillTreeColors::NodeBody::Default;
}

EVisibility SEdNode_SkillNode::GetDragOverMarkerVisibility() const
{
	return EVisibility::Visible;
}

const FSlateBrush* SEdNode_SkillNode::GetNameIcon() const
{
	return FAppStyle::GetBrush(TEXT("BTEditor.Graph.BTNode.Icon"));
}

#undef LOCTEXT_NAMESPACE