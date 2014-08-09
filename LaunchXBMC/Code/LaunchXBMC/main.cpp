#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>
using namespace std;

/*
	Input has to be a dynamically allocated array.

	Return the number of processes put in the DWORD array.
	Returns negative numbers if it failed.
*/
int GetListOfProcesses(DWORD* processes, DWORD listSize)
{
	DWORD sizeReturned;
	DWORD listSizeBytes;
	bool foundAllProcesses;

	//Find all processes.
	do
	{
		foundAllProcesses = true;
		//Calculate the byte size of the array.
		listSizeBytes = listSize * sizeof(DWORD);

		if (!EnumProcesses(processes, listSizeBytes, &sizeReturned))
		{
			return -1;
		}

		//If number of processes returned is equal to the size of the list.
		//Increase the size of the list and try again to see if there are any more processes.
		if (listSizeBytes == sizeReturned)
		{
			foundAllProcesses = false;
			listSize *= 2;
			delete[] processes;
			processes = new DWORD[listSize];
		}

	} while (!foundAllProcesses);

	//Calculate the number of processes in the array.
	int numberOfProcesses = sizeReturned / sizeof(DWORD);

	return numberOfProcesses;
}

bool StartProgram(wstring filename)
{
	STARTUPINFO startInfo = { 0 };
	PROCESS_INFORMATION pInfo = { 0 };

	//Zero the memory of both structures
	ZeroMemory(&startInfo, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
	ZeroMemory(&pInfo, sizeof(pInfo));

	//Try to start process
	if (!CreateProcess(filename.c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startInfo, &pInfo))
	{
		//Return false if process couldn't be created
		DWORD err = GetLastError();
		return false;
	}

	//Wait for the process to exit
	WaitForSingleObject(pInfo.hProcess, INFINITE);

	//Close both handles for the process
	CloseHandle(pInfo.hProcess);
	CloseHandle(pInfo.hThread);

	return true;
}

//Returns the position of the process in the array.
//Return -1 if it can't be found.
int FindProcess(DWORD* processes, int numberOfProcesses, wstring processName)
{
	TCHAR processNameTemp[MAX_PATH] = TEXT("<unknown>");

	for (int i = 0; i < numberOfProcesses; i++)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
		if (hProcess != NULL)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
				&cbNeeded))
			{
				GetModuleBaseName(hProcess, hMod, processNameTemp,
					sizeof(processNameTemp) / sizeof(TCHAR));

				if (!wcscmp(processNameTemp, processName.c_str()))
				{
					//Process found!
					CloseHandle(hProcess);
					return i;
				}
			}
		}
		CloseHandle(hProcess);
	}

	return -1;
}
/*
int main(void)
{/*
	DWORD* processes;
	DWORD listSize = 100;
	
	processes = new DWORD[listSize];

	// Get the list of process identifiers.
	int numberOfProcesses = GetListOfProcesses(processes, listSize);

	if (numberOfProcesses < 0)
	{
		return 0;
	}

	StartProgram(L"C:\\Program Files (x86)\\XBMC\\XBMC.exe");

	
	if (FindProcess(processes, numberOfProcesses, L"XBMC.exe") == -1)
	{
	}

	delete[] processes;
	
	return 0;
}
*/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	StartProgram(L"C:\\Program Files (x86)\\XBMC\\XBMC.exe");

	return 0;
}