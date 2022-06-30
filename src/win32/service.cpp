#include "stdafx.h"
#include "service.h"
#include "logger.h"
#include "internet_connection_status.h"

static const std::string g_csServiceName = "SystemAnalizer";

VOID WINAPI ServiceMain(DWORD dwArgc, LPSTR* lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
VOID SetServiceStatus(DWORD dwState, DWORD dwWin32ExitCode = NO_ERROR, DWORD dwWaitHint = 0UL);

static SERVICE_STATUS g_ServiceStatus = { 0 };
static SERVICE_STATUS_HANDLE g_hServiceStatus = nullptr;
static std::promise<void> g_StopPromise;

bool CService::Run()
{
	SERVICE_TABLE_ENTRY serviceTableEntry[] =
	{
		{ const_cast<LPSTR>(g_csServiceName.c_str()), ServiceMain },
		{ nullptr, nullptr }
	};

	return StartServiceCtrlDispatcher(serviceTableEntry) == TRUE;
}

void CService::Install()
{
	SC_HANDLE schSCManager = nullptr;
	SC_HANDLE schService = nullptr;

	do
	{
		char szPath[MAX_PATH] = { 0 };

		if (!GetModuleFileName(nullptr, szPath, MAX_PATH))
		{
			std::cout << "GetModuleFileName failed " << GetLastError() << std::endl;
			break;
		}

		schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
		if (!schSCManager)
		{
			std::cout << "OpenSCManager failed " << GetLastError() << std::endl;
			break;
		}

		schService = CreateService(
			schSCManager,
			g_csServiceName.c_str(),
			g_csServiceName.c_str(),
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
			std::cout << "CreateService failed " << GetLastError();
			break;
		}

		std::cout << "Service installed successfully" << std::endl;

	} while (false);

	if (schService)
	{
		CloseServiceHandle(schService);
		schService = nullptr;
	}

	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = nullptr;
	}
}

void CService::Uninstall()
{
	SC_HANDLE schSCManager = nullptr;
	SC_HANDLE schService = nullptr;

	do
	{
		schSCManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CONNECT);
		if (!schSCManager)
		{
			std::cout << "OpenSCManager failed " << GetLastError() << std::endl;
			break;
		}

		schService = OpenService(schSCManager, g_csServiceName.c_str(), SERVICE_ALL_ACCESS);
		if (!schService)
		{
			std::cout << "OpenSCManager failed " << GetLastError() << std::endl;
			break;
		}

		if (DeleteService(schService) == FALSE)
		{
			std::cout << "DeleteService failed " << GetLastError() << std::endl;
			break;
		}

		std::cout << "Service uninstalled successfully" << std::endl;
	} while (false);

	if (schService)
	{
		CloseServiceHandle(schService);
		schService = nullptr;
	}

	if (schSCManager)
	{
		CloseServiceHandle(schSCManager);
		schSCManager = nullptr;
	}
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPSTR* lpszArgv)
{
	g_hServiceStatus = RegisterServiceCtrlHandler(g_csServiceName.c_str(), ServiceCtrlHandler);
	if (g_hServiceStatus == nullptr)
	{
		return;
	}

	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwServiceSpecificExitCode = 0UL;

	SetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	CLogger::Init("D:/log.txt");
	CInternetConnectionStatus internetConnectionStatus;
	std::future<void> shouldStop = g_StopPromise.get_future();

	LOG() << "Starting";

	internetConnectionStatus.Start();

	SetServiceStatus(SERVICE_RUNNING);

	while (shouldStop.wait_for(std::chrono::milliseconds(500)) == std::future_status::timeout)
	{
	}


	internetConnectionStatus.StopAndWait();

	LOG() << "End\n\n";

	SetServiceStatus(SERVICE_STOPPED);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP:
		SetServiceStatus(SERVICE_STOP_PENDING);
		g_StopPromise.set_value();

		break;

	default:
		break;
	}
}

VOID SetServiceStatus(DWORD dwState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	g_ServiceStatus.dwCurrentState = dwState;
	g_ServiceStatus.dwWin32ExitCode = dwWin32ExitCode;
	g_ServiceStatus.dwWaitHint = dwWaitHint;

	if (dwState == SERVICE_RUNNING || dwState == SERVICE_STOPPED)
	{
		dwCheckPoint = 0;
	}
	else
	{
		dwCheckPoint = dwCheckPoint++;
	}

	::SetServiceStatus(g_hServiceStatus, &g_ServiceStatus);
}
