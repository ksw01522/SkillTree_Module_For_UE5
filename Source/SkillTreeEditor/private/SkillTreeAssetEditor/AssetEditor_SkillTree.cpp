// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTreeAssetEditor/AssetEditor_SkillTree.h"
#include "SkillTreeEditor.h"
#include "SkillTreeAssetEditor/AssetEditorToolbar_SkillTree.h"
#include "SkillTreeAssetEditor/EdGraphSchema_SkillTree.h"
#include "SkillTreeAssetEditor/EditorCommands_SkillTree.h"
#include "SkillTreeAssetEditor/EdGraph_SkillTree.h"
#include "AssetToolsModule.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraphUtilities.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillNode.h"
#include "SkillTreeAssetEditor/EdGraphNode_SkillTreeEdge.h"
#include "AutoLayout/TreeLayoutStrategy_STE.h"
#include "AutoLayout/ForceDirectedLayoutStrategy_STE.h"

#define LOCTEXT_NAMESPACE "AssetEditor_SkillTree"

const FName SkillTreeEditorAppName = FName(TEXT("SkillTreeEditorApp"));

//////////////////////////////////////////////////////////////////////////

struct FSkillTreeAssetEditorTabs
{
	// Tab identifiers
	static const FName SkillTreePropertyID;
	static const FName ViewportID;
	static const FName SkillTreeEditorSettingsID;
};

const FName FSkillTreeAssetEditorTabs::SkillTreePropertyID(TEXT("SkillTreeProperty"));
const FName FSkillTreeAssetEditorTabs::ViewportID(TEXT("Viewport"));
const FName FSkillTreeAssetEditorTabs::SkillTreeEditorSettingsID(TEXT("SkillTreeEditorSettings"));

//////////////////////////////////////////////////////////////////////////

FAssetEditor_SkillTree::FAssetEditor_SkillTree()
{
	EditingGraph = nullptr;
	
	EdGraphSubclass = UEdGraph_SkillTree::StaticClass();
	EdGraphSchemaSubclass = UEdGraphSchema_SkillTree::StaticClass();

	SkillTreeEditorSettings = NewObject<USkillTreeEditorSettings>(USkillTreeEditorSettings::StaticClass());

#if ENGINE_MAJOR_VERSION < 5
	OnPackageSavedDelegateHandle = UPackage::PackageSavedEvent.AddRaw(this, &FAssetEditor_SkillTree::OnPackageSaved);
#else // #if ENGINE_MAJOR_VERSION < 5
	OnPackageSavedDelegateHandle = UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAssetEditor_SkillTree::OnPackageSavedWithContext);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5
}

FAssetEditor_SkillTree::~FAssetEditor_SkillTree()
{
#if ENGINE_MAJOR_VERSION < 5
	UPackage::PackageSavedEvent.Remove(OnPackageSavedDelegateHandle);
#else // #if ENGINE_MAJOR_VERSION < 5
	UPackage::PackageSavedWithContextEvent.Remove(OnPackageSavedDelegateHandle);
#endif // #else // #if ENGINE_MAJOR_VERSION < 5
}

void FAssetEditor_SkillTree::InitSkillTreeAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, USkillTree* Graph)
{
	EditingGraph = Graph;
	CreateEdGraph();

	FGenericCommands::Register();
	FGraphEditorCommands::Register();
	FEditorCommands_SkillTree::Register();

	if (!ToolbarBuilder.IsValid())
	{
		ToolbarBuilder = MakeShareable(new FAssetEditorToolbar_SkillTree(SharedThis(this)));
	}

	BindCommands();

	CreateInternalWidgets();

	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);

	ToolbarBuilder->AddSkillTreeToolbar(ToolbarExtender);

	// Layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_SkillTreeEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
#if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)->SetHideTabWell(true)
			)
#endif // #if ENGINE_MAJOR_VERSION < 5
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.65f)
					->AddTab(FSkillTreeAssetEditorTabs::ViewportID, ETabState::OpenedTab)->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->AddTab(FSkillTreeAssetEditorTabs::SkillTreePropertyID, ETabState::OpenedTab)->SetHideTabWell(true)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.3f)
						->AddTab(FSkillTreeAssetEditorTabs::SkillTreeEditorSettingsID, ETabState::OpenedTab)
					)
				)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, SkillTreeEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, EditingGraph, false);

	RegenerateMenusAndToolbars();
}

void FAssetEditor_SkillTree::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_SkillTreeEditor", "Skill Tree Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(FSkillTreeAssetEditorTabs::ViewportID, FOnSpawnTab::CreateSP(this, &FAssetEditor_SkillTree::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(FSkillTreeAssetEditorTabs::SkillTreePropertyID, FOnSpawnTab::CreateSP(this, &FAssetEditor_SkillTree::SpawnTab_Details))
		.SetDisplayName(LOCTEXT("DetailsTab", "Property"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(FSkillTreeAssetEditorTabs::SkillTreeEditorSettingsID, FOnSpawnTab::CreateSP(this, &FAssetEditor_SkillTree::SpawnTab_EditorSettings))
		.SetDisplayName(LOCTEXT("EditorSettingsTab", "Custom Graph Editor Setttings"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FAssetEditor_SkillTree::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(FSkillTreeAssetEditorTabs::ViewportID);
	InTabManager->UnregisterTabSpawner(FSkillTreeAssetEditorTabs::SkillTreePropertyID);
	InTabManager->UnregisterTabSpawner(FSkillTreeAssetEditorTabs::SkillTreeEditorSettingsID);
}

FName FAssetEditor_SkillTree::GetToolkitFName() const
{
	return FName("FSkillTreeEditor");
}

FText FAssetEditor_SkillTree::GetBaseToolkitName() const
{
	return LOCTEXT("SkillTreeEditorAppLabel", "Custom Graph Editor");
}

FText FAssetEditor_SkillTree::GetToolkitName() const
{
	const bool bDirtyState = EditingGraph->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("SkillTreeName"), FText::FromString(EditingGraph->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("SkillTreeEditorToolkitName", "{SkillTreeName}{DirtyState}"), Args);
}

FText FAssetEditor_SkillTree::GetToolkitToolTipText() const
{
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingGraph);
}

FLinearColor FAssetEditor_SkillTree::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_SkillTree::GetWorldCentricTabPrefix() const
{
	return TEXT("SkillTreeEditor");
}

FString FAssetEditor_SkillTree::GetDocumentationLink() const
{
	return TEXT("");
}

void FAssetEditor_SkillTree::SaveAsset_Execute()
{
	if (EditingGraph != nullptr)
	{
		RebuildSkillTree();
	}

	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_SkillTree::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingGraph);
	Collector.AddReferencedObject(EditingGraph->EdGraph);
}

USkillTreeEditorSettings* FAssetEditor_SkillTree::GetSettings() const
{
	return SkillTreeEditorSettings;
}

TSharedRef<SDockTab> FAssetEditor_SkillTree::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FSkillTreeAssetEditorTabs::ViewportID);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (ViewportWidget.IsValid())
	{
		SpawnedTab->SetContent(ViewportWidget.ToSharedRef());
	}

	return SpawnedTab;
}

TSharedRef<SDockTab> FAssetEditor_SkillTree::SpawnTab_Details(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FSkillTreeAssetEditorTabs::SkillTreePropertyID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("Details_Title", "Property"))
		[
			PropertyWidget.ToSharedRef()
		];
}

TSharedRef<SDockTab> FAssetEditor_SkillTree::SpawnTab_EditorSettings(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == FSkillTreeAssetEditorTabs::SkillTreeEditorSettingsID);

	return SNew(SDockTab)
#if ENGINE_MAJOR_VERSION < 5
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
#endif // #if ENGINE_MAJOR_VERSION < 5
		.Label(LOCTEXT("EditorSettings_Title", "Custom Graph Editor Setttings"))
		[
			EditorSettingsWidget.ToSharedRef()
		];
}

FText FAssetEditor_SkillTree::GetCornerText()
{
	return LOCTEXT("AppearanceCornerText_SkillTree", "Custom Graph");
}

void FAssetEditor_SkillTree::CreateInternalWidgets()
{
	ViewportWidget = CreateViewportWidget();

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;
	Args.NotifyHook = this;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(EditingGraph);
	PropertyWidget->OnFinishedChangingProperties().AddSP(this, &FAssetEditor_SkillTree::OnFinishedChangingProperties);

	EditorSettingsWidget = PropertyModule.CreateDetailView(Args);
	EditorSettingsWidget->SetObject(SkillTreeEditorSettings);
}

TSharedRef<SGraphEditor> FAssetEditor_SkillTree::CreateViewportWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = GetCornerText();

	CreateCommandList();

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_SkillTree::OnSelectedNodesChanged);
	InEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(this, &FAssetEditor_SkillTree::OnNodeDoubleClicked);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingGraph->EdGraph)
		.GraphEvents(InEvents)
		.AutoExpandActionMenu(true)
		.ShowGraphStateOverlay(false);
}

void FAssetEditor_SkillTree::BindCommands()
{
	ToolkitCommands->MapAction(FEditorCommands_SkillTree::Get().GraphSettings,
		FExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::GraphSettings),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::CanGraphSettings)
	);

	ToolkitCommands->MapAction(FEditorCommands_SkillTree::Get().AutoArrange,
		FExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::AutoArrange),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::CanAutoArrange)
	);
}

void FAssetEditor_SkillTree::CreateEdGraph()
{
	if (EditingGraph->EdGraph == nullptr)
	{
		EditingGraph->EdGraph = CastChecked<UEdGraph_SkillTree>(FBlueprintEditorUtils::CreateNewGraph(EditingGraph, NAME_None, EdGraphSubclass, EdGraphSchemaSubclass));
		EditingGraph->EdGraph->bAllowDeletion = false;

		// Give the schema a chance to fill out any required nodes (like the results node)
		const UEdGraphSchema* Schema = EditingGraph->EdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*EditingGraph->EdGraph);
	}
}

void FAssetEditor_SkillTree::CreateCommandList()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}

	GraphEditorCommands = MakeShareable(new FUICommandList);

	// Can't use CreateSP here because derived editor are already implementing TSharedFromThis<FAssetEditorToolkit>
	// however it should be safe, since commands are being used only within this editor
	// if it ever crashes, this function will have to go away and be reimplemented in each derived class

	GraphEditorCommands->MapAction(FEditorCommands_SkillTree::Get().GraphSettings,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::GraphSettings),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanGraphSettings));

	GraphEditorCommands->MapAction(FEditorCommands_SkillTree::Get().AutoArrange,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::AutoArrange),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanAutoArrange));

	GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::SelectAllNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanSelectAllNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanDeleteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CopySelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanCopyNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CutSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanCutNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::PasteNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanPasteNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
		FExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::DuplicateNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_SkillTree::CanDuplicateNodes)
	);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Rename,
		FExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::OnRenameNode),
		FCanExecuteAction::CreateSP(this, &FAssetEditor_SkillTree::CanRenameNodes)
	);
}

TSharedPtr<SGraphEditor> FAssetEditor_SkillTree::GetCurrGraphEditor() const
{
	return ViewportWidget;
}

FGraphPanelSelectionSet FAssetEditor_SkillTree::GetSelectedNodes() const
{
	FGraphPanelSelectionSet CurrentSelection;
	TSharedPtr<SGraphEditor> FocusedGraphEd = GetCurrGraphEditor();
	if (FocusedGraphEd.IsValid())
	{
		CurrentSelection = FocusedGraphEd->GetSelectedNodes();
	}

	return CurrentSelection;
}

void FAssetEditor_SkillTree::RebuildSkillTree()
{
	if (EditingGraph == nullptr)
	{
		LOG_WARNING(TEXT("RebuildSkillTree EditingGraph is nullptr"));
		return;
	}

	UEdGraph_SkillTree* EdGraph = Cast<UEdGraph_SkillTree>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	EdGraph->RebuildSkillTree();
}

void FAssetEditor_SkillTree::SelectAllNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		CurrentGraphEditor->SelectAllNodes();
	}
}

bool FAssetEditor_SkillTree::CanSelectAllNodes()
{
	return true;
}

void FAssetEditor_SkillTree::DeleteSelectedNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentGraphEditor->GetCurrentGraph()->Modify();

	const FGraphPanelSelectionSet SelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
	{
		UEdGraphNode* EdNode = Cast<UEdGraphNode>(*NodeIt);
		if (EdNode == nullptr || !EdNode->CanUserDeleteNode())
			continue;;

		if (UEdGraphNode_SkillNode* EdNode_Node = Cast<UEdGraphNode_SkillNode>(EdNode))
		{
			EdNode_Node->Modify();

			const UEdGraphSchema* Schema = EdNode_Node->GetSchema();
			if (Schema != nullptr)
			{
				Schema->BreakNodeLinks(*EdNode_Node);
			}

			EdNode_Node->DestroyNode();
		}
		else
		{
			EdNode->Modify();
			EdNode->DestroyNode();
		}
	}
}

bool FAssetEditor_SkillTree::CanDeleteNodes()
{
	// If any of the nodes can be deleted then we should allow deleting
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node != nullptr && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_SkillTree::DeleteSelectedDuplicatableNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}

	const FGraphPanelSelectionSet OldSelectedNodes = CurrentGraphEditor->GetSelectedNodes();
	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}

	// Delete the duplicatable nodes
	DeleteSelectedNodes();

	CurrentGraphEditor->ClearSelectionSet();

	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter))
		{
			CurrentGraphEditor->SetNodeSelection(Node, true);
		}
	}
}

void FAssetEditor_SkillTree::CutSelectedNodes()
{
	CopySelectedNodes();
	DeleteSelectedDuplicatableNodes();
}

bool FAssetEditor_SkillTree::CanCutNodes()
{
	return CanCopyNodes() && CanDeleteNodes();
}

void FAssetEditor_SkillTree::CopySelectedNodes()
{
	// Export the selected nodes and place the text on the clipboard
	FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();

	FString ExportedText;

	for (FGraphPanelSelectionSet::TIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node == nullptr)
		{
			SelectedIter.RemoveCurrent();
			continue;
		}

		if (UEdGraphNode_SkillTreeEdge* EdNode_Edge = Cast<UEdGraphNode_SkillTreeEdge>(*SelectedIter))
		{
			UEdGraphNode_SkillNode* StartNode = EdNode_Edge->GetStartNode();
			UEdGraphNode_SkillNode* EndNode = EdNode_Edge->GetEndNode();

			if (!SelectedNodes.Contains(StartNode) || !SelectedNodes.Contains(EndNode))
			{
				SelectedIter.RemoveCurrent();
				continue;
			}
		}

		Node->PrepareForCopying();
	}

	FEdGraphUtilities::ExportNodesToText(SelectedNodes, ExportedText);
	FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FAssetEditor_SkillTree::CanCopyNodes()
{
	// If any of the nodes can be duplicated then we should allow copying
	const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanDuplicateNode())
		{
			return true;
		}
	}

	return false;
}

void FAssetEditor_SkillTree::PasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		PasteNodesHere(CurrentGraphEditor->GetPasteLocation());
	}
}

void FAssetEditor_SkillTree::PasteNodesHere(const FVector2D& Location)
{
	// Find the graph editor with focus
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return;
	}
	// Select the newly pasted stuff
	UEdGraph* EdGraph = CurrentGraphEditor->GetCurrentGraph();

	{
		const FScopedTransaction Transaction(FGenericCommands::Get().Paste->GetDescription());
		EdGraph->Modify();

		// Clear the selection set (newly pasted stuff will be selected)
		CurrentGraphEditor->ClearSelectionSet();

		// Grab the text to paste from the clipboard.
		FString TextToImport;
		FPlatformApplicationMisc::ClipboardPaste(TextToImport);

		// Import the nodes
		TSet<UEdGraphNode*> PastedNodes;
		FEdGraphUtilities::ImportNodesFromText(EdGraph, TextToImport, PastedNodes);

		//Average position of nodes so we can move them while still maintaining relative distances to each other
		FVector2D AvgNodePosition(0.0f, 0.0f);

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			AvgNodePosition.X += Node->NodePosX;
			AvgNodePosition.Y += Node->NodePosY;
		}

		float InvNumNodes = 1.0f / float(PastedNodes.Num());
		AvgNodePosition.X *= InvNumNodes;
		AvgNodePosition.Y *= InvNumNodes;

		for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
		{
			UEdGraphNode* Node = *It;
			CurrentGraphEditor->SetNodeSelection(Node, true);

			Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
			Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

			Node->SnapToGrid(16);

			// Give new node a different Guid from the old one
			Node->CreateNewGuid();
		}
	}

	// Update UI
	CurrentGraphEditor->NotifyGraphChanged();

	UObject* GraphOwner = EdGraph->GetOuter();
	if (GraphOwner)
	{
		GraphOwner->PostEditChange();
		GraphOwner->MarkPackageDirty();
	}
}

bool FAssetEditor_SkillTree::CanPasteNodes()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (!CurrentGraphEditor.IsValid())
	{
		return false;
	}

	FString ClipboardContent;
	FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

	return FEdGraphUtilities::CanImportNodesFromText(CurrentGraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FAssetEditor_SkillTree::DuplicateNodes()
{
	CopySelectedNodes();
	PasteNodes();
}

bool FAssetEditor_SkillTree::CanDuplicateNodes()
{
	return CanCopyNodes();
}

void FAssetEditor_SkillTree::GraphSettings()
{
	PropertyWidget->SetObject(EditingGraph);
}

bool FAssetEditor_SkillTree::CanGraphSettings() const
{
	return true;
}

void FAssetEditor_SkillTree::AutoArrange()
{
	UEdGraph_SkillTree* EdGraph = Cast<UEdGraph_SkillTree>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	const FScopedTransaction Transaction(LOCTEXT("SkillTreeEditorAutoArrange", "Custom Graph Editor: Auto Arrange"));

	EdGraph->Modify();

	UAutoLayoutStrategy_STE* LayoutStrategy = nullptr;
	switch (SkillTreeEditorSettings->AutoLayoutStrategy)
	{
	case EAutoLayoutStrategy_STE::Tree:
		LayoutStrategy = NewObject<UAutoLayoutStrategy_STE>(EdGraph, UTreeLayoutStrategy_STE::StaticClass());
		break;
	case EAutoLayoutStrategy_STE::ForceDirected:
		LayoutStrategy = NewObject<UAutoLayoutStrategy_STE>(EdGraph, UForceDirectedLayoutStrategy_STE::StaticClass());
		break;
	default:
		break;
	}

	if (LayoutStrategy != nullptr)
	{
		LayoutStrategy->Settings = SkillTreeEditorSettings;
		LayoutStrategy->Layout(EdGraph);
		LayoutStrategy->ConditionalBeginDestroy();
	}
	else
	{
		LOG_ERROR(TEXT("FAssetEditor_SkillTree::AutoArrange LayoutStrategy is null."));
	}
}

bool FAssetEditor_SkillTree::CanAutoArrange() const
{
	return EditingGraph != nullptr && Cast<UEdGraph_SkillTree>(EditingGraph->EdGraph) != nullptr;
}

void FAssetEditor_SkillTree::OnRenameNode()
{
	TSharedPtr<SGraphEditor> CurrentGraphEditor = GetCurrGraphEditor();
	if (CurrentGraphEditor.IsValid())
	{
		const FGraphPanelSelectionSet SelectedNodes = GetSelectedNodes();
		for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt)
		{
			UEdGraphNode* SelectedNode = Cast<UEdGraphNode>(*NodeIt);
			if (SelectedNode != NULL && SelectedNode->bCanRenameNode)
			{
				CurrentGraphEditor->IsNodeTitleVisible(SelectedNode, true);
				break;
			}
		}
	}
}

bool FAssetEditor_SkillTree::CanRenameNodes() const
{
	UEdGraph_SkillTree* EdGraph = Cast<UEdGraph_SkillTree>(EditingGraph->EdGraph);
	check(EdGraph != nullptr);

	USkillTree* Graph = EdGraph->GetSkillTree();
	check(Graph != nullptr)

		return false;//Graph->bCanRenameNode && GetSelectedNodes().Num() == 1;
}

void FAssetEditor_SkillTree::OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection)
{
	TArray<UObject*> Selection;

	for (UObject* SelectionEntry : NewSelection)
	{
		Selection.Add(SelectionEntry);
	}

	if (Selection.Num() == 0)
	{
		PropertyWidget->SetObject(EditingGraph);

	}
	else
	{
		PropertyWidget->SetObjects(Selection);
	}
}

void FAssetEditor_SkillTree::OnNodeDoubleClicked(UEdGraphNode* Node)
{

}

void FAssetEditor_SkillTree::OnFinishedChangingProperties(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (EditingGraph == nullptr)
		return;

	EditingGraph->EdGraph->GetSchema()->ForceVisualizationCacheClear();
}

#if ENGINE_MAJOR_VERSION < 5
void FAssetEditor_SkillTree::OnPackageSaved(const FString& PackageFileName, UObject* Outer)
{
	RebuildSkillTree();
}
#else // #if ENGINE_MAJOR_VERSION < 5
void FAssetEditor_SkillTree::OnPackageSavedWithContext(const FString& PackageFileName, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	RebuildSkillTree();
}
#endif // #else // #if ENGINE_MAJOR_VERSION < 5

void FAssetEditor_SkillTree::RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}


#undef LOCTEXT_NAMESPACE

