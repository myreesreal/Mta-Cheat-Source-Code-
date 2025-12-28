#include <includes.h>
#include <menu/menu.h>
#include <netc/netc.h>
#include <client/client.h>
#include <data/variables.h>
#include <console/console.h>
#include <data/elements.h>
void Initialize()
{
	console->initialize();

	var->winapi.mh_status = MH_Initialize();
	if (var->winapi.mh_status != MH_OK)
	{
		console->error("Failed to initialize MinHook: %d", var->winapi.mh_status);
		return;
	}



	if (!netc->release())
	{
		console->error("Failed to initialize network hooks\n");
		return;
	}

	if (!menu->release())
	{
		console->error("Failed to initialize DirectX hooks\n");
		return;
	}

	std::thread([&]()
		{
			while (true)
			{
				HMODULE hClient = GetModuleHandleA("client.dll");
				if (hClient != nullptr)
				{
					if (!element->content.loaded_client)
					{
						client->release();
					}
				}
				else
				{
					if (element->content.loaded_client)
					{
						client->shutdown();
					}
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		}
	).detach();
}

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	{
		var->winapi.hModule = hModule;
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Initialize, nullptr, 0, nullptr);
		break;
	}
	}

	return TRUE;
}
