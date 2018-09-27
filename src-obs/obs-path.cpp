#include "stdafx.h"
#include "obs-path.h"
#include "CqtUtil.h"
#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
using namespace std;

bool checkNecessaryclientPath()
{
	if (!DirectoryExistsW(getObsDataDirPath()))
		return false;

	if (!DirectoryExistsW(getObsPluginsPath()))
		return false;

	if (!ForceDirectoryW(getObsPluginCfgPath()))
		return false;

	if (!ForceDirectoryW(getObsBasicPath()))
		return false;
	
	if (!ForceDirectoryW(getObsProfilerDataPath()))
		return false;

	if (!ForceDirectoryW(getObsUpdatesPath()))
		return false;

	if (!ForceDirectoryW(getObsCrashesPath()))
		return false;

	if (!ForceDirectoryW(getObsLogsPath()))
		return false;

	if (!ForceDirectoryW(getObsBasicPath()))
		return false;

	return true;
}

std::wstring getObsDataDirPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\", IncludeTrailingPathDelimiterW(GetExeRunPath()).c_str(),L"data");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsPluginsPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\", IncludeTrailingPathDelimiterW(GetExeRunPath()).c_str(), L"plugins");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsBasicDirPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"basic");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsCrashesPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win" ,L"crashes");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsLogsPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"logs");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsPluginCfgPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"plugin_config");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsProfilerDataPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"profiler_data");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsUpdatesPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"updates");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsBasicPath()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s\\", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"basic");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring getObsPathGlobalConfig()
{
	wchar_t wch[MAX_PATH] = { 0 };
	_snwprintf_s(wch, MAX_PATH, _TRUNCATE, L"%s%s\\%s", IncludeTrailingPathDelimiterW(GetAppDataPath()).c_str(), L"obs-win", L"global.ini");
	std::wstring wstpath(wch);
	return wstpath;
}

std::wstring GetDefaultVideoSavePath()
{
	wchar_t path_utf16[MAX_PATH];
	SHGetFolderPathW(NULL, CSIDL_MYVIDEO, NULL, SHGFP_TYPE_CURRENT, path_utf16);
	return wstring(path_utf16);
}
