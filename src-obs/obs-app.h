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
	bool _InitGlobalConfig();
	bool _InitLocale();
private:
	ConfigFile globalConfig;
	profiler_name_store_t* profilerNameStore_;
};

