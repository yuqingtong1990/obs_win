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
	ConfigFile     globalConfig;
	TextLookup     textLookup;
	OBSContext     obsContext;
};

