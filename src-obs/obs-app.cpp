#include "stdafx.h"
#include "obs-app.h"
#include "obs-path.h"
#include "profiler.hpp"

static auto ProfilerNameStoreRelease = [](profiler_name_store_t *store){
	profiler_name_store_free(store);
};
using ProfilerNameStore = std::unique_ptr<profiler_name_store_t,decltype(ProfilerNameStoreRelease)>;
ProfilerNameStore CreateNameStore(){
	return ProfilerNameStore{ profiler_name_store_create(),ProfilerNameStoreRelease };
}

OBSApp::OBSApp()
{
}


OBSApp::~OBSApp()
{
}

bool OBSApp::Initialize()
{
	if (!checkNecessaryclientPath())
		return false;

	auto profilerNameStore = CreateNameStore();
// 	profiler_start();
// 	profile_register_root("run_program_init", 0);
// 	ScopeProfiler prof{ "run_program_init" };
	profilerNameStore_ = profilerNameStore.get();
	//"zh-CN"
	
}

void OBSApp::Destory()
{

}

bool OBSApp::InitGlobalConfig()
{
	int errorcode = globalConfig.Open(_W2A_(getObsPathGlobalConfig(),CP_UTF8), CONFIG_OPEN_ALWAYS);
	if (errorcode != CONFIG_SUCCESS) {
		return false;
	}
}
