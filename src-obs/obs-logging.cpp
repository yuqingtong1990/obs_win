#include "stdafx.h"
#include "obs-path.h"
#include "obs-logging.h"
#include "util/base.h"
#include <fstream>




using namespace std;
#define MAX_CRASH_REPORT_SIZE (150 * 1024)
static void main_crash_handler(const char *format, va_list args, void *param)
{
	char *text = new char[MAX_CRASH_REPORT_SIZE];
	vsnprintf(text, MAX_CRASH_REPORT_SIZE, format, args);
	text[MAX_CRASH_REPORT_SIZE - 1] = '\0';

	wchar_t wpath[MAX_PATH] = { 0 };
	SYSTEMTIME stNow;
	::GetLocalTime(&stNow);
	_snwprintf_s(wpath, MAX_PATH, _TRUNCATE, L"%s%s.dmp",getObsCrashesPath().c_str(), FormatDatetimeW(stNow).c_str());
	fstream file;
	file.open(wpath, std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
	file << text;
	delete[] text;
	file.close();
	exit(-1);
	UNUSED_PARAMETER(param);
}

static void load_debug_privilege(void)
{
	const DWORD flags = TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY;
	TOKEN_PRIVILEGES tp;
	HANDLE token;
	LUID val;

	if (!OpenProcessToken(GetCurrentProcess(), flags, &token)) {
		return;
	}

	if (!!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &val)) {
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = val;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(token, false, &tp,
			sizeof(tp), NULL, NULL);
	}

	CloseHandle(token);
}

void obs_win32crash_init()
{
	obs_init_win32_crash_handler();
	SetErrorMode(SEM_FAILCRITICALERRORS);
	load_debug_privilege();
	base_set_crash_handler(main_crash_handler, nullptr);
}

//base_get_log_handler(&def_log_handler, nullptr);
void obs_logging_init()
{
	std::string strlog = _W2A_(getObsLogsPath(), CP_UTF8).c_str();
	FLAGS_log_dir = strlog;
	std::vector<std::string> lst;
	GetCommandLineList(lst);
	google::InitGoogleLogging("obswin");
}

void obs_logging_end()
{
	google::ShutdownGoogleLogging();
}
