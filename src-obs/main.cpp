
#include "stdafx.h"
#include <DbgHelp.h>
#include "obs-app.h"



int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	OBSApp::get().Initialize();
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
