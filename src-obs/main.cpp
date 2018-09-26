
#include "stdafx.h"
#include <DbgHelp.h>
#include "obs-app.h"
#include "obs-logging.h"
#include "obs-path.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	//确保安装目录存在
	if (!checkNecessaryclientPath())
		return -1;

	obs_init_win32_crash_handler();
	obs_logging_init();
	OBSApp::get().Initialize();
	while (true)
	{
		Sleep(100);
	}
	obs_logging_end();
	return 0;
}


void CreateDumpFile(LPCWSTR lpDumpFilePath, EXCEPTION_POINTERS *pE) {

	HANDLE hDumpF = ::CreateFile(lpDumpFilePath, GENERIC_WRITE, 0,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pE;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = FALSE;

	::MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpF, MiniDumpNormal,
		&dumpInfo, NULL, NULL);

	::CloseHandle(hDumpF);
}

LONG AppCrashHandleCallback(EXCEPTION_POINTERS *pE){
	//提示程序挂了，并上传dump
	return EXCEPTION_EXECUTE_HANDLER;
}
