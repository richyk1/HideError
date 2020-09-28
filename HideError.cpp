#include <iostream>
#include <Windows.h>
#include <Psapi.h>


DWORD_PTR GetProcessBaseAddress(HANDLE processHandle)
{
	DWORD_PTR   baseAddress = 0;
	HMODULE* moduleArray;
	LPBYTE      moduleArrayBytes;
	DWORD       bytesRequired;

	if (processHandle)
	{
		if (EnumProcessModules(processHandle, NULL, 0, &bytesRequired))
		{
			if (bytesRequired)
			{
				moduleArrayBytes = (LPBYTE)LocalAlloc(LPTR, bytesRequired);

				if (moduleArrayBytes)
				{
					unsigned int moduleCount;

					moduleCount = bytesRequired / sizeof(HMODULE);
					moduleArray = (HMODULE*)moduleArrayBytes;

					if (EnumProcessModules(processHandle, moduleArray, bytesRequired, &bytesRequired))
					{
						baseAddress = (DWORD_PTR)moduleArray[0];
					}

					LocalFree(moduleArrayBytes);
				}
			}
		}
	}
	return baseAddress;
}

int main()
{
	HWND window;

	while (true)
	{

		auto window = FindWindowA(NULL, "Europa Universalis IV");
		if (window)
		{
			std::cout << "Found window" << std::endl;
			DWORD procID;
			GetWindowThreadProcessId(window, &procID);
			std::cout << "ProcID" << procID << std::endl;
			if (procID)
			{
				auto handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
				if (handle)
				{
					std::cout << "Handle: " << handle << std::endl;

					auto baseAddr = GetProcessBaseAddress(handle);
					const BYTE arr[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
					auto result = WriteProcessMemory(handle, (LPVOID)(baseAddr + 0x7654E4), arr, sizeof(arr), NULL); // FF 15 ? ? ? ? FF 15 ? ? ? ? 85 C0 
					if (result == 0)
						std::cout << GetLastError() << std::endl;
				}
				CloseHandle(handle);
			}
		}
		Sleep(3000);
	}
}