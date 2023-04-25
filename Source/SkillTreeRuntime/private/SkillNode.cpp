// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillNode.h"
#include "SkillTreeEdge.h"
#include "SkillAbility.h"
#include "LearnSkillCondition.h"

#define LOCTEXT_NAMESPACE "SkillNode"

USkillNode::USkillNode()
{
#if WITH_EDITORONLY_DATA
    ContextMenuName = LOCTEXT("Skill Node Context Menu Name", "New Skill Node");
#endif
}

FText USkillNode::GetSkillName() const
{
    if(SkillInNode.GetDefaultObject() == nullptr) return LOCTEXT("Empty Skill Node Name", "스킬이 지정되어 있지 않습니다.");

    return SkillInNode.GetDefaultObject()->GetSkillName();
}

FText USkillNode::GetSkillDescription() const
{
    if (SkillInNode.GetDefaultObject() == nullptr) return FText::GetEmpty();

    return SkillInNode.GetDefaultObject()->GetSkillDescription();
}

bool USkillNode::CanLearnSkill(UAbilitySystemComponent* AS, int32 TargetLv, UPARAM(Ref) FText& CantReason) const
{
    if (SkillInNode.GetDefaultObject() == nullptr) {
        CantReason = LOCTEXT("Can't Earn Empty Skill Reason", "지정되어 있는 스킬이 없어 배울 수 없습니다. 개발 오류입니다.");
        return false;
    }

    bool FinalResult = true;
    TArray<FText> Reasons; 
    FText ReasonHead = LOCTEXT("Can't Earn Skill Reason Head", "다음과 같은 이유로 스킬을 배울 수 없습니다.");
    Reasons.Add(ReasonHead);

    for (auto Condition : LearnSkillConditions)
    {
        FText Reason;
        bool Result = Condition->IsCanLearn(AS, TargetLv, Reason);
        FinalResult = FinalResult && Result;

        if (!Result) {
            Reasons.Add(Reason);
        }
    }

    if(!FinalResult)   CantReason = FText::Join(FText::FromString("\n"), Reasons);

    return FinalResult;
}

USkillTreeEdge* USkillNode::GetEdge(USkillNode* Node)
{
    if(USkillTreeEdge** TreeEdgePtr = Edges.Find(Node)) return *TreeEdgePtr;

    return nullptr;
}

FText USkillNode::GetNodeTitle() const
{
    return GetSkillName();
}

void USkillNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    OnSkillNodePropertyChanged.Broadcast(PropertyChangedEvent);
}

#undef LOCTEXT_NAMESPACE