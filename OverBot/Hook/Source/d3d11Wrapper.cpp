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
	: hD3D(nullptr), bIsDllValid(false), wsaPort("8008"), res(nullptr), bIsBound(false), m_device(nullptr),
	  m_swapchain(nullptr)
{
	Event.open("D3D11.log");
	Event << LOG("Initialising") << std::endl;

	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "DLL initialised" << std::endl;
}

D3D11Wrapper::~D3D11Wrapper()
{
	Event.close();
}


[[nodiscard]]
bool D3D11Wrapper::CreateSocketAndBind()
{
	// Windows Socket returns 0 on success...
	if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		this->Event << LOGERR("WSAStartup Failed.") << std::endl;
		return false;
	}

	addrinfo *ptr = nullptr, hints;

	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = IPPROTO_TCP; // USING TCP FOR NOW TODO CHANGE
	hints.ai_flags = AI_PASSIVE;

	// Get Local addr
	auto err = getaddrinfo(nullptr, wsaPort.c_str(), &hints, &res);
	if (err)
	{
		Event << LOGERR("Unable to get address info.") << std::endl;
		WSACleanup();
		return false;
	}

	SOCKET ServerSocket = INVALID_SOCKET;
	ServerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) 
	{
		Event << LOGERR("Error with Socket ") << WSAGetLastError() << std::endl;
		freeaddrinfo(res);
		WSACleanup();
		return false;
	}

	err = bind(ServerSocket, res->ai_addr, int(res->ai_addrlen));
	if(err == SOCKET_ERROR)
	{
		Event << LOGERR("Unable to bind") << std::endl;
		freeaddrinfo(res);
		closesocket(ServerSocket);
		WSACleanup();
		return false;
	}

	bIsBound = true;
	return true;
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

void D3D11Wrapper::setDevice(ID3D11Device* dev)
{
	m_device = dev;
	//Event << "Device is " << ((m_device != nullptr) ? "VALID" : "INVALID") << std::endl;
}

void D3D11Wrapper::setSwapChain(IDXGISwapChain* swapchain)
{
	m_swapchain = swapchain;
}
