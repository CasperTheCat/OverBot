#include "../Header/d3d11Wrapper.h"
#include "../Header/utils.h"
#include <iostream>
#include <minwinbase.h>
//#include <ws2def.h>


D3D11Wrapper::D3D11Wrapper(HMODULE _hD3D)
	: hD3D(_hD3D), bIsDllValid(true)
{
	//Event = std::ofstream("d3d.txt");
}

D3D11Wrapper::D3D11Wrapper()
	: hD3D(nullptr), bIsDllValid(false), m_device(nullptr),
	  m_swapchain(nullptr), hComms(nullptr)
{
	Event.open("D3D11.log");
	Event << LOG("Initialising") << std::endl;

	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "DLL initialised" << std::endl;
}

D3D11Wrapper::~D3D11Wrapper()
{
	if(!hComms) delete hComms;
	Event.close();
}


[[nodiscard]]
int D3D11Wrapper::CreateSocketAndBind()
{
	if (!hComms) return EXIT_FAILURE;
	//if (!hComms) hComms = new HookCommunications();

	return hComms->CreateSocketAndBind();
}

void D3D11Wrapper::RegisterNetwork(HookCommunications *hCom)
{
	hComms = hCom;
	//hComms->CleanAndReset();
}

bool D3D11Wrapper::LoadDLL()
{
	// Initialise wrapper
	Event << LOG("Loading DLL") << std::endl;

	HMODULE hD3D = nullptr;
	if (IsWow64())
	{
		Event << LOG("Running on SysWOW64") << std::endl;
		hD3D = LoadLibrary(L"C:\\Windows\\SysWOW64\\d3d11.dll");
	}
	else
	{
		hD3D = LoadLibrary(L"C:\\Windows\\System32\\d3d11.dll");
	}

	if (hD3D == NULL)
	{
		Event << LOGERR("Unable to load DLL") << std::endl;
		return false;
	}

	this->hD3D = hD3D;
	this->bIsDllValid = true;
	Event << LOG("Loaded DLL") << std::endl;
	return true;
}

HMODULE D3D11Wrapper::getDLL()
{
	if (!bIsDllValid) this->LoadDLL();
	return hD3D;

}

HookCommunications *D3D11Wrapper::GetHookComms()
{
	return hComms;
}

void D3D11Wrapper::setDevice(ID3D11Device* dev)
{
	m_device = dev;
	//Event << "Device is " << ((m_device != nullptr) ? "VALID" : "INVALID") << std::endl;
}

void D3D11Wrapper::setSwapChain(IDXGISwapChain* swapchain)
{
	m_swapchain = swapchain;
}
