#include <cstdio>
#include <Windows.h>

#define SERVICE_NAME "MyService"

VOID InstallService(LPCSTR pszServiceName);
VOID UninstallService(LPCSTR lpszServiceName);

VOID WINAPI ServiceMain(DWORD dwArgc, LPSTR* lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
VOID SetServiceStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode = NO_ERROR,
	DWORD dwWaitHint = 0);
VOID ServiceStart(DWORD dwArgc, LPSTR* lpszArgv);

SERVICE_STATUS g_ServiceStatus = {};
SERVICE_STATUS_HANDLE g_hServiceStatus = nullptr;
HANDLE g_hServiceStopEvent = nullptr;

INT main(INT iArgc, LPSTR lpszArgv[])
{
	if (iArgc == 2)
	{
		if (strcmp(lpszArgv[1], "/install") == 0)
		{
			InstallService(SERVICE_NAME);
		}
		else if (strcmp(lpszArgv[1], "/uninstall") == 0)
		{
			UninstallService(SERVICE_NAME);
		}
		else
		{
			printf_s("Unknown command");
		}
		return 0;
	}

	SERVICE_TABLE_ENTRY serviceTableEntry[] =
	{
		{ const_cast<LPSTR>(SERVICE_NAME), ServiceMain },
		{ nullptr, nullptr }
	};

	DWORD dwError = 0;
	if (StartServiceCtrlDispatcher(serviceTableEntry) == FALSE)
	{
		dwError = GetLastError();
		if (dwError == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
		{
			printf_s("Can`t run service application as console application");
		}
		return -1;
	}
	return 0;
}

VOID InstallService(LPCSTR lpszServiceName)
{
	SC_HANDLE schSCManager = nullptr;
	SC_HANDLE schService = nullptr;
	CHAR szPath[MAX_PATH] = {};

	if (!GetModuleFileName(nullptr, szPath, MAX_PATH))
	{
		printf_s("GetModuleFileName failed %d", GetLastError());
	}

	schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	if (!schSCManager)
	{
		printf_s("OpenSCManager failed %d", GetLastError());
		return;
	}

	schService = CreateService(
		schSCManager,
		lpszServiceName,
		lpszServiceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szPath,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr);
	if (!schService)
	{
		printf_s("CreateService failed %d", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	printf_s("Service installed successfully");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID UninstallService(LPCSTR lpszServiceName)
{
	SC_HANDLE schSCManager = nullptr;
	SC_HANDLE schService = nullptr;

	schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (schSCManager == nullptr)
	{
		printf_s("OpenSCManager failed %d", GetLastError());
		return;
	}

	schService = OpenService(
		schSCManager,
		lpszServiceName,
		SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
	if (schService == nullptr)
	{
		printf_s("OpenSCManager failed %d", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	if (!DeleteService(schService))
	{
		printf_s("DeleteService failed %d", GetLastError());
	}
	else
	{
		printf_s("Service uninstalled successfully");
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

VOID SetServiceStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	g_ServiceStatus.dwCurrentState = dwCurrentState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_RUNNING || dwCurrentState == SERVICE_STOPPED)
	{
		dwCheckPoint = 0;
	}
	else
	{
		dwCheckPoint = dwCheckPoint++;
	}

	::SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPSTR* lpszArgv)
{
	g_hServiceStatus = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	if (g_hServiceStatus == nullptr)
	{
		return;
	}

	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;

	SetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	ServiceStart(dwArgc, lpszArgv);
}

VOID ServiceStart(DWORD dwArgc, LPSTR* lpszArgv)
{
	g_hServiceStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	if (g_hServiceStopEvent == nullptr)
	{
		SetServiceStatus(SERVICE_STOPPED);
		return;
	}

	SetServiceStatus(SERVICE_RUNNING);

	while (true)
	{
		if (WaitForSingleObject(g_hServiceStopEvent, 0) == WAIT_OBJECT_0)
		{
			SetServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
			return;
		}

		FILE* file = nullptr;
		fopen_s(&file, "C:/log.txt", "a+");

		if (file != nullptr)
		{
			fprintf_s(file, "Hello world\n");
			fclose(file);
		}

		Sleep(3000);
	}
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		SetServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		SetEvent(g_hServiceStopEvent);

		break;

	default:
		break;
	}
}
