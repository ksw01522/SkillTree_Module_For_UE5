// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/EdGraphSchema_SkillTree.h"
#include "ToolMenus.h"
#include "SkillTreeEditor.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"
#include "SkillTreeAssetEditor/ConnectionDrawingPolicy_SkillTree.h"
#include "GraphEditorActions.h"
#include "Framework/Commands/GenericCommands.h"
#include "AutoLayout/ForceDirectedLayoutStrategy_STE.h"
#include "AutoLayout/TreeLayoutStrategy_STE.h"
#include "SkillNode.h"

#define LOCTEXT_NAMESPACE "EdSchema_SkillTree"

int32 UEdGraphSchema_SkillTree::CurrentCacheRefreshID = 0;

class FNodeVisitorCycleChecker
{
public:
	/** Check whether a loop in the graph would be caused by linking the passed-in nodes */
	bool CheckForLoop(UEdGraphNode* StartNode, UEdGraphNode* EndNode) 
	{
		VisitedNodes.Add(StartNode);

		return TraverseNodes(EndNode);
	}

private:
	bool TraverseNodes(UEdGraphNode* Node)
	{
		VisitedNodes.Add(Node);

		for (auto MyPin : Node->Pins)
		{
			if (MyPin->Direction == EGPD_Output)
			{
				for (auto OtherPin : MyPin->LinkedTo)
				{
					UEdGraphNode* OtherNode = OtherPin->GetOwningNode();
					if (VisitedNodes.Contains(OtherNode))
					{
						// Only  an issue if this is a back-edge
						return false;
					}
					else if (!FinishedNodes.Contains(OtherNode))
					{
						// Only should traverse if this node hasn't been traversed
						if (!TraverseNodes(OtherNode))
							return false;
					}
				}
			}
		}

		VisitedNodes.Remove(Node);
		FinishedNodes.Add(Node);
		return true;
	};


	TSet<UEdGraphNode*> VisitedNodes;
	TSet<UEdGraphNode*> FinishedNodes;
};

UEdGraphNode* FEdSchemaAction_SkillTree_NewNode::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("SkillTreeEditorNewNode", "Custom Graph Editor : New Node"));
		ParentGraph->Modify();
		if(FromPin != nullptr) FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SkillNode->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
 	}

	return ResultNode;
}

void FEdSchemaAction_SkillTree_NewNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);

}

UEdGraphNode* FEdSchemaAction_SkillTree_NewEdge::PerformAction(UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode* ResultNode = nullptr;

	if (NodeTemplate != nullptr)
	{
		const FScopedTransaction Transaction(LOCTEXT("SkillTreeEditorNewNode", "Custom Graph Editor : New Edge"));
		ParentGraph->Modify();
		if (FromPin != nullptr) FromPin->Modify();

		NodeTemplate->Rename(nullptr, ParentGraph);
		ParentGraph->AddNode(NodeTemplate, true, bSelectNewNode);

		NodeTemplate->CreateNewGuid();
		NodeTemplate->PostPlacedNewNode();
		NodeTemplate->AllocateDefaultPins();
		NodeTemplate->AutowireNewNode(FromPin);

		NodeTemplate->NodePosX = Location.X;
		NodeTemplate->NodePosY = Location.Y;

		NodeTemplate->SkillTreeEdge->SetFlags(RF_Transactional);
		NodeTemplate->SetFlags(RF_Transactional);

		ResultNode = NodeTemplate;
	}

	return ResultNode;
}

void FEdSchemaAction_SkillTree_NewEdge::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdGraphSchemaAction::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(NodeTemplate);

}

void UEdGraphSchema_SkillTree::GetBreakLinkToSubMenuActions(class UToolMenu* Menu, class UEdGraphPin* InGraphPin)
{
	// Make sure we have a unique name for every entry in the list
	TMap< FString, uint32 > LinkTitleCount;

	FToolMenuSection& Section = Menu->FindOrAddSection("SkillTreeSchemaPinActions");

	// Add all the links we could break from
	for (TArray<class UEdGraphPin*>::TConstIterator Links(InGraphPin->LinkedTo); Links; ++Links)
	{
		UEdGraphPin* Pin = *Links;
		FString TitleString = Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::ListView).ToString();
		FText Title = FText::FromString(TitleString);
		if (Pin->PinName != TEXT(""))
		{
			TitleString = FString::Printf(TEXT("%s (%s)"), *TitleString, *Pin->PinName.ToString());

			// Add name of connection if possible
			FFormatNamedArguments Args;
			Args.Add(TEXT("NodeTitle"), Title);
			Args.Add(TEXT("PinName"), Pin->GetDisplayName());
			Title = FText::Format(LOCTEXT("BreakDescPin", "{NodeTitle} ({PinName})"), Args);
		}

		uint32& Count = LinkTitleCount.FindOrAdd(TitleString);

		FText Description;
		FFormatNamedArguments Args;
		Args.Add(TEXT("NodeTitle"), Title);
		Args.Add(TEXT("NumberOfNodes"), Count);

		if (Count == 0)
		{
			Description = FText::Format(LOCTEXT("BreakDesc", "Break link to {NodeTitle}"), Args);
		}
		else
		{
			Description = FText::Format(LOCTEXT("BreakDescMulti", "Break link to {NodeTitle} ({NumberOfNodes})"), Args);
		}
		++Count;

		Section.AddMenuEntry(NAME_None, Description, Description, FSlateIcon(), FUIAction(
			FExecuteAction::CreateUObject(this, &UEdGraphSchema_SkillTree::BreakSinglePinLink, const_cast<UEdGraphPin*>(InGraphPin), *Links)));
	}
}

EGraphType UEdGraphSchema_SkillTree::GetGraphType(const UEdGraph* TestEdGraph) const
{
	return EGraphType::GT_StateMachine;
}

void UEdGraphSchema_SkillTree::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	USkillTree* SkillTree = CastChecked<USkillTree>(ContextMenuBuilder.CurrentGraph->GetOuter());

	const bool bNoParent = (ContextMenuBuilder.FromPin == NULL);

	const FText AddToolTip = LOCTEXT("New Skill Tree Node Tooltip", "Add node here");

	TSet<TSubclassOf<USkillNode> > Visited;

	TSubclassOf<USkillNode> SkillClass = USkillNode::StaticClass();
	FText Desc;

	if (!SkillClass->HasAnyClassFlags(CLASS_Abstract))
	{
		Desc = SkillClass.GetDefaultObject()->ContextMenuName;
		TSharedPtr<FEdSchemaAction_SkillTree_NewNode> NewNodeAction(new FEdSchemaAction_SkillTree_NewNode(GetNodeCategoryName(), Desc, AddToolTip, 0));
		NewNodeAction->NodeTemplate = NewObject<UEdGraphNode_SkillNode>(ContextMenuBuilder.OwnerOfTemporaries);
		NewNodeAction->NodeTemplate->SetSkillNode( NewObject<USkillNode>(NewNodeAction->NodeTemplate));
		NewNodeAction->NodeTemplate->SkillNode->InTree = SkillTree;
		ContextMenuBuilder.AddAction(NewNodeAction);

		Visited.Add(SkillClass);
	}

	for (TObjectIterator<UClass> It; It; ++It)
	{
		if (!It->HasAnyClassFlags(EClassFlags::CLASS_Abstract) && It->IsChildOf(SkillClass) && !Visited.Contains(*It))
		{
			if(It->GetName().StartsWith("REINST") || It->GetName().StartsWith("SKEL"))	continue;

			TSubclassOf<USkillNode> NodeType = *It;
			Desc = NodeType.GetDefaultObject()->ContextMenuName;

			if (Desc.IsEmpty())
			{
				FString Title = NodeType->GetName();
				Title.RemoveFromEnd("_C");
				Desc = FText::FromString(Title);
			}

			TSharedPtr<FEdSchemaAction_SkillTree_NewNode> ChildNewNodeAction(new FEdSchemaAction_SkillTree_NewNode(GetNodeCategoryName(), Desc, AddToolTip, 0));
			ChildNewNodeAction->NodeTemplate = NewObject<UEdGraphNode_SkillNode>(ContextMenuBuilder.OwnerOfTemporaries);
			ChildNewNodeAction->NodeTemplate->SetSkillNode( NewObject<USkillNode>(ChildNewNodeAction->NodeTemplate, NodeType));
			ChildNewNodeAction->NodeTemplate->SkillNode->InTree = SkillTree;
			ContextMenuBuilder.AddAction(ChildNewNodeAction);

			Visited.Add(NodeType);
		}
	}
}

void UEdGraphSchema_SkillTree::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	/*
	if (Context->Pin)
	{
		{
			FToolMenuSection& Section = Menu->AddSection("SkillTreeSchemaNodeActions", LOCTEXT("PinActionsMenuHeader", "Pin Actions"));

			if (Context->Pin->LinkedTo.Num() > 0)
			{
				if (Context->Pin->LinkedTo.Num() > 1)
				{
					Section.AddSubMenu(
						"BreakLinkTo",
						LOCTEXT("BreakLinkTo", "Break Link To..."),
						LOCTEXT("BreakSpecificLinks", "Break a specific link..."),
						FNewToolMenuDelegate::CreateUObject((UEdGraphSchema_SkillTree* const)this, &UEdGraphSchema_SkillTree::GetBreakLinkToSubMenuActions, const_cast<UEdGraphPin*>(Context->Pin)));
				}
				else
				{
					((UEdGraphSchema_SkillTree* const)this)->GetBreakLinkToSubMenuActions(Menu, const_cast<UEdGraphPin*>(Context->Pin));
				}
			}
		}
	}
	else*/ if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("SkillTreeSchemaNodeActions", LOCTEXT("ClassActionsMenuHeader", "Node Actions"));
		Section.AddMenuEntry(FGenericCommands::Get().Delete);
		Section.AddMenuEntry(FGenericCommands::Get().Cut);
		Section.AddMenuEntry(FGenericCommands::Get().Copy);
		Section.AddMenuEntry(FGenericCommands::Get().Duplicate);

		Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
	}

	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UEdGraphSchema_SkillTree::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorSameNode", "Can't connect node to itself"));
	}

	const UEdGraphPin* Out = A;
	const UEdGraphPin* In = B;

	UEdGraphNode_SkillNode* EdNode_Out = Cast<UEdGraphNode_SkillNode>(Out->GetOwningNode());
	UEdGraphNode_SkillNode* EdNode_In = Cast<UEdGraphNode_SkillNode>(In->GetOwningNode());

	if (EdNode_Out == nullptr || EdNode_In == nullptr)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinError", "Not a valid USkillTreeEdNode"));
	}

	//Determine if we can have cycles or not
	bool bAllowCycles = false;
	auto EdGraph = Cast<UEdGraph_SkillTree>(Out->GetOwningNode()->GetGraph());
	if (EdGraph != nullptr)
	{
		//bAllowCycles = EdGraph->GetSkillTree()->bCanBeCyclical;
	}

	// check for cycles
	FNodeVisitorCycleChecker CycleChecker;
	if (!bAllowCycles && !CycleChecker.CheckForLoop(Out->GetOwningNode(), In->GetOwningNode()))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, LOCTEXT("PinErrorCycle", "Can't create a graph cycle"));
	}

	FText ErrorMessage;
	if (!EdNode_Out->SkillNode->CanCreateConnectionTo(EdNode_In->SkillNode, EdNode_Out->GetOutputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}
	if (!EdNode_In->SkillNode->CanCreateConnectionFrom(EdNode_Out->SkillNode, EdNode_In->GetInputPin()->LinkedTo.Num(), ErrorMessage))
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, ErrorMessage);
	}


	if (true)//EdNode_Out->SkillNode->GetSkillTree()->bEdgeEnabled)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE_WITH_CONVERSION_NODE, LOCTEXT("PinConnect", "Connect nodes with edge"));
	}
	else
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, LOCTEXT("PinConnect", "Connect nodes"));
	}
}

bool UEdGraphSchema_SkillTree::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphNode_SkillNode* NodeA = Cast<UEdGraphNode_SkillNode>(A->GetOwningNode());
	UEdGraphNode_SkillNode* NodeB = Cast<UEdGraphNode_SkillNode>(B->GetOwningNode());

	// Check that this edge doesn't already exist
	for (UEdGraphPin* TestPin : NodeA->GetOutputPin()->LinkedTo)
	{
		UEdGraphNode* ChildNode = TestPin->GetOwningNode();
		if (UEdGraphNode_SkillTreeEdge* EdNode_Edge = Cast<UEdGraphNode_SkillTreeEdge>(ChildNode))
		{
			ChildNode = EdNode_Edge->GetEndNode();
		}

		if (ChildNode == NodeB)
			return false;
	}

	if (NodeA && NodeB)
	{
		// Always create connections from node A to B, don't allow adding in reverse
		Super::TryCreateConnection(NodeA->GetOutputPin(), NodeB->GetInputPin());
		return true;
	}
	else
	{
		return false;
	}
}

bool UEdGraphSchema_SkillTree::CreateAutomaticConversionNodeAndConnections(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraphNode_SkillNode* NodeA = Cast<UEdGraphNode_SkillNode>(A->GetOwningNode());
	UEdGraphNode_SkillNode* NodeB = Cast<UEdGraphNode_SkillNode>(B->GetOwningNode());

	// Are nodes and pins all valid?
	if (!NodeA || !NodeA->GetOutputPin() || !NodeB || !NodeB->GetInputPin())
		return false;

	USkillTree* Graph = NodeA->SkillNode->GetSkillTree();

	FVector2D InitPos((NodeA->NodePosX + NodeB->NodePosX) / 2, (NodeA->NodePosY + NodeB->NodePosY) / 2);

	FEdSchemaAction_SkillTree_NewEdge Action;
	Action.NodeTemplate = NewObject<UEdGraphNode_SkillTreeEdge>(NodeA->GetGraph());
	Action.NodeTemplate->SetEdge(NewObject<USkillTreeEdge>(Action.NodeTemplate));
	UEdGraphNode_SkillTreeEdge* EdgeNode = Cast<UEdGraphNode_SkillTreeEdge>(Action.PerformAction(NodeA->GetGraph(), nullptr, InitPos, false));

	// Always create connections from node A to B, don't allow adding in reverse
	EdgeNode->CreateConnections(NodeA, NodeB);

	return true;
}

FConnectionDrawingPolicy* UEdGraphSchema_SkillTree::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const
{
	return new FConnectionDrawingPolicy_SkillTree(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

FLinearColor UEdGraphSchema_SkillTree::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	return FColor::White;
}

void UEdGraphSchema_SkillTree::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakNodeLinks", "Break Node Links"));

	Super::BreakNodeLinks(TargetNode);

}

void UEdGraphSchema_SkillTree::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakPinLinks", "Break Pin Links"));

	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UEdGraphSchema_SkillTree::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "GraphEd_BreakSinglePinLink", "Break Pin Link"));

	Super::BreakSinglePinLink(SourcePin, TargetPin);

}

UEdGraphPin* UEdGraphSchema_SkillTree::DropPinOnNode(UEdGraphNode* InTargetNode, const FName& InSourcePinName, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection) const
{
	UEdGraphNode_SkillNode* EdNode = Cast<UEdGraphNode_SkillNode>(InTargetNode);
	switch (InSourcePinDirection)
	{
		case EGPD_Input:
			return EdNode->GetOutputPin();
		case EGPD_Output:
			return EdNode->GetInputPin();
		default:
			return nullptr;
	}
}

bool UEdGraphSchema_SkillTree::SupportsDropPinOnNode(UEdGraphNode* InTargetNode, const FEdGraphPinType& InSourcePinType, EEdGraphPinDirection InSourcePinDirection, FText& OutErrorMessage) const
{
	return Cast<UEdGraphNode_SkillNode>(InTargetNode) != nullptr;
}

bool UEdGraphSchema_SkillTree::IsCacheVisualizationOutOfDate(int32 InVisualizationCacheID) const
{
	return CurrentCacheRefreshID != InVisualizationCacheID;
}

int32 UEdGraphSchema_SkillTree::GetCurrentVisualizationCacheID() const
{
	return CurrentCacheRefreshID;
}

void UEdGraphSchema_SkillTree::ForceVisualizationCacheClear() const
{
	++CurrentCacheRefreshID;
}



FText UEdGraphSchema_SkillTree::GetNodeCategoryName() const
{
	return LOCTEXT("Skill Node Action", "Skill Node");
}

#undef LOCTEXT_NAMESPACE