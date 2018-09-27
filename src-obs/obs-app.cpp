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

	_InitOBSCallbacks();
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

void OBSApp::SceneReordered(void *data, calldata_t *params)
{
//	OBSApp *app = static_cast<OBSApp*>(app);
	obs_scene_t *scene = (obs_scene_t*)calldata_ptr(params, "scene");
}

void OBSApp::SceneItemAdded(void *data, calldata_t *params)
{
//	OBSApp *app = static_cast<OBSApp*>(app);
	obs_sceneitem_t *scene = (obs_sceneitem_t*)calldata_ptr(params, "item");
}

void OBSApp::SceneItemSelected(void *data, calldata_t *params)
{
//	OBSApp *app = static_cast<OBSApp*>(app);

	obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
	obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
}

void OBSApp::SceneItemDeselected(void *data, calldata_t *params)
{
	//OBSApp *app = static_cast<OBSApp*>(data);

	obs_scene_t     *scene = (obs_scene_t*)calldata_ptr(params, "scene");
	obs_sceneitem_t *item = (obs_sceneitem_t*)calldata_ptr(params, "item");
}

void OBSApp::SourceCreated(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");

	if (obs_scene_from_source(source) != NULL) {

	}
}

void OBSApp::SourceRemoved(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");

	if (obs_scene_from_source(source) != NULL) {

	}
}

void OBSApp::SourceActivated(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
	uint32_t     flags = obs_source_get_output_flags(source);

	if (flags & OBS_SOURCE_AUDIO) {
	}
}

void OBSApp::SourceDeactivated(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
	uint32_t     flags = obs_source_get_output_flags(source);

	if (flags & OBS_SOURCE_AUDIO) {

	}
}

void OBSApp::SourceRenamed(void *data, calldata_t *params)
{
	obs_source_t *source = (obs_source_t*)calldata_ptr(params, "source");
	const char *newName = calldata_string(params, "new_name");
	const char *prevName = calldata_string(params, "prev_name");
}

void OBSApp::RenderMain(void *data, uint32_t cx, uint32_t cy)
{

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
	config_set_bool(globalConfig, "Audio", "DisableAudioDucking", true);
	globalConfig.Save();
	return true;
}

bool OBSApp::_InitLocale()
{
	//多语言以后再实现
	return true;
}

void OBSApp::_InitOBSCallbacks()
{
	ProfileScope("OBSBasic::InitOBSCallbacks");
	signalHandlers.reserve(signalHandlers.size() + 6);
	signalHandlers.emplace_back(obs_get_signal_handler(), "source_create", OBSApp::SourceCreated, this);
	signalHandlers.emplace_back(obs_get_signal_handler(), "source_remove", OBSApp::SourceRemoved, this);
	signalHandlers.emplace_back(obs_get_signal_handler(), "source_activate",OBSApp::SourceActivated, this);
	signalHandlers.emplace_back(obs_get_signal_handler(), "source_deactivate",OBSApp::SourceDeactivated, this);
	signalHandlers.emplace_back(obs_get_signal_handler(), "source_rename",OBSApp::SourceRenamed, this);
}

bool OBSApp::_InitBasicConfigDefaults()
{
	return true;
}

bool OBSApp::_ResetAudio()
{
	ProfileScope("OBSBasic::ResetAudio");

	struct obs_audio_info ai;
	ai.samples_per_sec = config_get_uint(basicConfig, "Audio","SampleRate");
	const char *channelSetupStr = config_get_string(basicConfig,"Audio", "ChannelSetup");

	if (strcmp(channelSetupStr, "Mono") == 0)
		ai.speakers = SPEAKERS_MONO;
	else if (strcmp(channelSetupStr, "2.1") == 0)
		ai.speakers = SPEAKERS_2POINT1;
	else if (strcmp(channelSetupStr, "4.0") == 0)
		ai.speakers = SPEAKERS_4POINT0;
	else if (strcmp(channelSetupStr, "4.1") == 0)
		ai.speakers = SPEAKERS_4POINT1;
	else if (strcmp(channelSetupStr, "5.1") == 0)
		ai.speakers = SPEAKERS_5POINT1;
	else if (strcmp(channelSetupStr, "7.1") == 0)
		ai.speakers = SPEAKERS_7POINT1;
	else
		ai.speakers = SPEAKERS_STEREO;

	return obs_reset_audio(&ai);
}

bool OBSApp::_ResetVideo()
{
	return true;
}
