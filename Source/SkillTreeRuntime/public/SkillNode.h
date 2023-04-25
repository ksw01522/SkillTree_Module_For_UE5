// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillNode.generated.h"

/**
 * 
 */

class USkillTree;
class USkillTreeEdge;
class ULearnSkillCondition;
class USkillAbility;
class UAbilitySystemComponent;

UCLASS(BlueprintType)
class SKILLTREERUNTIME_API USkillNode : public UObject
{
	GENERATED_BODY()
	
public:
	USkillNode();
	virtual ~USkillNode() {}

	UPROPERTY()
	USkillTree* InTree;

	UPROPERTY()
	TArray<USkillNode*> ParentNodes;

	UPROPERTY()
	TArray<USkillNode*> ChildNodes;

	UPROPERTY()
	TMap<USkillNode*, USkillTreeEdge*> Edges;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Skill")
	TArray<ULearnSkillCondition*> LearnSkillConditions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill")
	TSubclassOf<USkillAbility> SkillInNode;

public:
/////////////////////////// Skill
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SkillNode")
	FText GetSkillName() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SkillNode")
	FText GetSkillDescription() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SkillNode")
	bool CanLearnSkill(UAbilitySystemComponent* AS, int32 TargetLv, UPARAM(Ref) FText& CantReason) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SkillNode")
	USkillTree* GetSkillTree(){return InTree;}

/////////////////////////// Node
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "SkillNode")
	virtual USkillTreeEdge* GetEdge(USkillNode* Node);


#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor")
	FLinearColor BackgroundColor;

	//노드 생성 이름
	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor")
	FText ContextMenuName;

	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor")
	bool bIsParentLimit;

	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor", meta = (ClampMin = "0", EditCondition = "bIsParentLimit", EditConditionHides))
	int32 ParentLimit;

	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor")
	bool bIsChildrenLimit;

	UPROPERTY(EditDefaultsOnly, Category = "SkillNode_Editor", meta = (ClampMin = "0", EditCondition = "bIsChildrenLimit", EditConditionHides))
	int32 ChildrenLimit;
	
#endif

#if WITH_EDITOR
	virtual FLinearColor GetBackgroundColor() const {return BackgroundColor;};

	virtual FText GetNodeTitle() const;

	virtual bool CanCreateConnection(USkillNode* Other, FText& ErrorMessage) {return true;}
	virtual bool CanCreateConnectionTo(USkillNode* Other, int32 NumberOfChildrenNodes, FText& ErrorMessage) { return true; }
	virtual bool CanCreateConnectionFrom(USkillNode* Other, int32 NumberOfParentNodes, FText& ErrorMessage) { return true; }

	virtual void RebuildNode() {};

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	DECLARE_EVENT_OneParam(USkillNode, FSkillNodePropertyChanged, const FPropertyChangedEvent& /* PropertyChangedEvent */);
	FSkillNodePropertyChanged OnSkillNodePropertyChanged;


#endif


};
