#include "SkillTreeAssetEditor/Settings_SkillTreeEditor.h"

USkillTreeEditorSettings::USkillTreeEditorSettings()
{
	AutoLayoutStrategy = EAutoLayoutStrategy_STE::Tree;

	bFirstPassOnly = false;

	bRandomInit = false;

	OptimalDistance = 100.f;

	MaxIteration = 50;

	InitTemperature = 10.f;

	CoolDownRate = 10.f;
}

USkillTreeEditorSettings::~USkillTreeEditorSettings()
{

}

