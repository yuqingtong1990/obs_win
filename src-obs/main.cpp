
#include "stdafx.h"
#include <DbgHelp.h>

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

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	obs_load_all_modules();
	return 0;
}


LONG AppCrashHandleCallback(EXCEPTION_POINTERS *pE){
	//提示程序挂了，并上传dump
	return EXCEPTION_EXECUTE_HANDLER;
}
