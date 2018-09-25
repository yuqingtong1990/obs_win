#pragma once
class OBSApp
{
public:
	OBSApp();
	~OBSApp();
public:
	bool Initialize();
	void Destory();
private:
	bool InitGlobalConfig();
private:
	ConfigFile     globalConfig;
	TextLookup     textLookup;
	OBSContext     obsContext;
	profiler_name_store_t* profilerNameStore_ = nullptr;
};

