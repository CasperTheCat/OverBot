#include "../Header/FrameComms.h"
#include <iostream>

int main(int argc, char **argv)
{
	std::string ipaddr = "127.0.0.1";

	if(argc > 1)
	{
		ipaddr = argv[1];
	}

	auto frameCommunications = new FrameCommunications();

	if(WAS_SUCCESS(frameCommunications->ConnectToHost(ipaddr)))
	{
		frameCommunications->Run();
	}

	std::cout << "Exiting" << std::endl;
	return 0;
}
