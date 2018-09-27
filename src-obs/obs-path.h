#pragma once

//检查必要的路径是否已经创建(没有创建会创建, 创建失败的话返回false)
bool checkNecessaryclientPath();


//获取data文件夹
std::wstring getObsDataDirPath();
//获取plugins文件夹
std::wstring getObsPluginsPath();

//获取Basic文件夹
std::wstring getObsBasicDirPath();
//获取Crashe存放文件夹
std::wstring getObsCrashesPath();
//获取logs存放文件夹
std::wstring getObsLogsPath();
//获取plugin_config存放文件夹
std::wstring getObsPluginCfgPath();
//获取profiler_data存放文件夹
std::wstring getObsProfilerDataPath();
//获取updates存放文件夹
std::wstring getObsUpdatesPath();
//获取basic存放文件夹
std::wstring getObsBasicPath();

//获取globalConfig存放文件夹
std::wstring getObsPathGlobalConfig();
//获取默认的视频存放路径
std::wstring GetDefaultVideoSavePath();
