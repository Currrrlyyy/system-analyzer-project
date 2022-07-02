#include "stdafx.h"
#include "service.h"
#include "utils.h"
#include "logger.h"
#include "config_parser.h"
#include "internet_connection_status.h"
#include "disk_status.h"

static const std::string g_csServiceName = "SystemAnalyzer";

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
	if (!g_hServiceStatus)
	{
		return;
	}

	char szDrive[MAX_PATH] = { 0 };
	char szPath[MAX_PATH] = { 0 };
	_splitpath(
		utils::GetServiceBinaryPath(g_csServiceName).value_or("").c_str(),
		szDrive,
		szPath,
		nullptr,
		nullptr);

	SetCurrentDirectory((std::string(szDrive) + std::string(szPath)).c_str());

	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwServiceSpecificExitCode = 0UL;

	SetServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000UL);

	CConfigParser config;
	std::ostringstream logBuffer;
	config.ParseConfigFile(logBuffer);

	CLogger::Init(config.GetLogPath() + config.GetLogName());
	LOG() << logBuffer.str();

	CInternetConnectionStatus internetConnectionStatus;
	CDiskStatus diskStatus(config.GetMinimalDeltaMB());
	std::future<void> shouldStop = g_StopPromise.get_future();

	LOG() << "\n >> Start";

	internetConnectionStatus.Start();
	diskStatus.Start();

	SetServiceStatus(SERVICE_RUNNING);

	shouldStop.wait();

	internetConnectionStatus.StopAndWait();
	diskStatus.StopAndWait();

	std::string endSymbolSeq(100, '*');
	LOG() << "\n >> End\n\n" << endSymbolSeq << "\n";
	

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
