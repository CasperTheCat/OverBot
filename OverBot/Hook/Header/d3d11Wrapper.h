#pragma once

// Socks
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#pragma comment(lib, "Ws2_32.lib")

#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_2.h>
#include <d3d11_3.h>
#include <d3d11_4.h>
#include <d3d11on12.h>
#include <fstream>



class D3D11Wrapper
{
protected:
	HMODULE hD3D;
	ID3D11Device *m_device;
	IDXGISwapChain* m_swapchain;
	bool bIsDllValid;
	std::string wsaPort;
	struct addrinfo *res;

	// Server Up?
	bool bIsBound;

public:
	std::ofstream Event;

	/**
	 * Windows Socket Data
	 */
	WSADATA wsaData;


protected:


public:
	D3D11Wrapper(HMODULE _hD3D);
	D3D11Wrapper();
	~D3D11Wrapper();

	/**
	 * Create Socket and Bind
	 */
	[[nodiscard]]
	bool CreateSocketAndBind();
	
	/// Public functions
	bool LoadDLL();

	// Getters
	HMODULE getDLL();

	// Setters
	void setDevice(ID3D11Device *dev);
	void setSwapChain(IDXGISwapChain* swapchain);
};
