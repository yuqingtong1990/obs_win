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
private:
	bool InitGlobalConfig();
private:
	ConfigFile     globalConfig;
	TextLookup     textLookup;
};

