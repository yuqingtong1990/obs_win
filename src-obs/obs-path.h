#pragma once

//����Ҫ��·���Ƿ��Ѿ�����(û�д����ᴴ��, ����ʧ�ܵĻ�����false)
bool checkNecessaryclientPath();


//��ȡdata�ļ���
std::wstring getObsDataDirPath();
//��ȡplugins�ļ���
std::wstring getObsPluginsPath();

//��ȡBasic�ļ���
std::wstring getObsBasicDirPath();
//��ȡCrashe����ļ���
std::wstring getObsCrashesPath();
//��ȡlogs����ļ���
std::wstring getObsLogsPath();
//��ȡplugin_config����ļ���
std::wstring getObsPluginCfgPath();
//��ȡprofiler_data����ļ���
std::wstring getObsProfilerDataPath();
//��ȡupdates����ļ���
std::wstring getObsUpdatesPath();
//��ȡbasic����ļ���
std::wstring getObsBasicPath();

//��ȡglobalConfig����ļ���
std::wstring getObsPathGlobalConfig();
//��ȡĬ�ϵ���Ƶ���·��
std::wstring GetDefaultVideoSavePath();
