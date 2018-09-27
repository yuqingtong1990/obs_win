#pragma once
class OBSApp
{
public:
	OBSApp();
	~OBSApp();
	static OBSApp& get();
public:
	bool Initialize();
	void Destory();
	std::string GetVersionString() const;
private:
	//obs»Øµ÷
	static void SceneReordered(void *data, calldata_t *params);
	static void SceneItemAdded(void *data, calldata_t *params);
	static void SceneItemSelected(void *data, calldata_t *params);
	static void SceneItemDeselected(void *data, calldata_t *params);
	static void SourceCreated(void *data, calldata_t *params);
	static void SourceRemoved(void *data, calldata_t *params);
	static void SourceActivated(void *data, calldata_t *params);
	static void SourceDeactivated(void *data, calldata_t *params);
	static void SourceRenamed(void *data, calldata_t *params);
	static void RenderMain(void *data, uint32_t cx, uint32_t cy);

	bool _InitGlobalConfig();
	bool _InitLocale();
	void _InitOBSCallbacks();
	bool _InitBasicConfigDefaults();
	bool _ResetAudio();
	bool _ResetVideo();
private:
	std::vector<OBSSignal> signalHandlers;
	ConfigFile globalConfig;
	ConfigFile basicConfig;
	profiler_name_store_t* profilerNameStore_;
};

