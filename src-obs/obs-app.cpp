#include "stdafx.h"
#include "obs-app.h"
#include "obs-path.h"
#include "profiler.hpp"

OBSApp::OBSApp()
{
}


OBSApp::~OBSApp()
{
	Destory();
}

OBSApp& OBSApp::get()
{
	static OBSApp s;
	return s;
}

bool OBSApp::Initialize()
{
	if (!_InitGlobalConfig())
		return false;

	if (!_InitLocale())
		return false;

	profilerNameStore_ = profiler_name_store_create();
	obs_startup("zh-CN", _W2A_(getObsPluginCfgPath(), CP_UTF8).c_str(), profilerNameStore_);

	LOG_INFO << obs_get_version_string();

	obs_load_all_modules();
	return true;
}

void OBSApp::Destory()
{
	if (profilerNameStore_)
		profiler_name_store_free(profilerNameStore_);
	
	obs_shutdown();
}

std::string OBSApp::GetVersionString() const
{
	return obs_get_version_string();
}

bool OBSApp::_InitGlobalConfig()
{
	LOG_INFO << "OBSApp::OBSInit";
	int errorcode = globalConfig.Open(_W2A_(getObsPathGlobalConfig(),CP_UTF8).c_str(), CONFIG_OPEN_ALWAYS);
	if (errorcode != CONFIG_SUCCESS) {
		LOG_ERROR << "globalConfig is error";
		return false;
	}	

	LOG_INFO << "set  set default global config";
	config_set_default_string(globalConfig, "General", "Language", "zh-CN");//默认中文
	config_set_default_uint(globalConfig, "General", "MaxLogs", 10);
	config_set_default_int(globalConfig, "General", "InfoIncrement", -1);
	config_set_default_string(globalConfig, "General", "ProcessPriority","Normal");
	config_set_default_bool(globalConfig, "General", "EnableAutoUpdates",false);
	config_set_default_string(globalConfig, "Video", "Renderer","Direct3D 11");
	config_set_default_bool(globalConfig, "BasicWindow", "PreviewEnabled",true);
	config_set_default_bool(globalConfig, "BasicWindow","PreviewProgramMode", false);
	config_set_default_bool(globalConfig, "BasicWindow","SceneDuplicationMode", true);
	config_set_default_bool(globalConfig, "BasicWindow","SwapScenesMode", true);
	config_set_default_bool(globalConfig, "BasicWindow","SnappingEnabled", true);
	config_set_default_bool(globalConfig, "BasicWindow","ScreenSnapping", true);
	config_set_default_bool(globalConfig, "BasicWindow","SourceSnapping", true);
	config_set_default_bool(globalConfig, "BasicWindow","CenterSnapping", false);
	config_set_default_double(globalConfig, "BasicWindow","SnapDistance", 10.0);
	config_set_default_bool(globalConfig, "BasicWindow","RecordWhenStreaming", false);
	config_set_default_bool(globalConfig, "BasicWindow","KeepRecordingWhenStreamStops", false);
	config_set_default_bool(globalConfig, "BasicWindow","SysTrayEnabled", true);
	config_set_default_bool(globalConfig, "BasicWindow","SysTrayWhenStarted", false);
	config_set_default_bool(globalConfig, "BasicWindow","SaveProjectors", false);
	config_set_default_bool(globalConfig, "BasicWindow","ShowTransitions", true);
	config_set_default_bool(globalConfig, "BasicWindow","ShowListboxToolbars", true);
	config_set_default_bool(globalConfig, "BasicWindow","ShowStatusBar", true);
	config_set_default_bool(globalConfig, "BasicWindow","VerticalVolControl", false);
	config_set_default_bool(globalConfig, "BasicWindow","MultiviewMouseSwitch", true);
	config_set_default_bool(globalConfig, "BasicWindow","MultiviewDrawNames", true);
	config_set_default_bool(globalConfig, "BasicWindow","MultiviewDrawAreas", true);
	config_set_default_bool(globalConfig, "Audio", "DisableAudioDucking", true);
	return true;
}

bool OBSApp::_InitLocale()
{
	//多语言以后再实现
	return true;
}
