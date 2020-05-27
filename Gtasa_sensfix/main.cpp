#include <windows.h>
#include <psapi.h>
#include <iostream>

HANDLE ProgramHandle;
DWORD HSens = 0xB6EC1C;
DWORD VSens = 0xB6EC18;
int iHSens = 0;
int iVSens = 0;
float fHSens = 0.0f;
float fVSens = 0.0f;
std::string strr("byw");

std::string GetProcessName(DWORD processID)
{
	CHAR szProcessName[MAX_PATH] = "<unknown>";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess)
	{
		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
			{
				if (GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(CHAR)) == 0)
				{
					//cout << "Error at GetModuleBaseName: " << GetLastError() << endl;
				}

				CloseHandle(hProcess);
				std::string str(szProcessName);
				return str;
			}
			else {
				//cout << "Error at EnumProcessModules: " << GetLastError() << endl;
			}
		}
		CloseHandle(hProcess);
	}

	return strr;
}

int FindProcess(std::string modsearchname)
{
	DWORD aProcesses[1024], cProcesses, cbNeeded;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 0;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			std::string modname = GetProcessName(aProcesses[i]);
			std::size_t found = modname.find(modsearchname);
			if (found != std::string::npos) {
				return aProcesses[i];
			}
		}
	}
	return 0;
}


int ReadInt(DWORD Addr)
{
	int val = 0;
	ReadProcessMemory(ProgramHandle, (void*)Addr, &val, sizeof(val), 0);
	
	return val;
}

void WriteFloat(DWORD Addr,float value)
{
	WriteProcessMemory(ProgramHandle, (void*)Addr, &value, sizeof(value), 0);
}

int main(int argc, char** argv)
{
	int choice = 0;
	float inputFloat = 0.0;
	printf("GTA SA:MP Sensitivity Fix\n");
	int ProcessID;
	bool same = false;
	ProcessID = FindProcess("gta_sa.exe");
	if (ProcessID)
	{
		//printf("gta_sa.exe PID: %i\n", ProcessID);
		ProgramHandle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
		if (!ProgramHandle)
		{
			printf("Could not open gta_sa.exe\n");
			exit(0);
		}
		else
		{
			//printf("gta_sa.exe handle: %i\n", ProgramHandle);
		}
	}
	Start:
	iVSens = ReadInt(VSens);
	iHSens = ReadInt(HSens);
	fVSens = *reinterpret_cast<float*>(&iVSens);
	fHSens = *reinterpret_cast<float*>(&iHSens);
	
	printf("Horizontal(x) sensitivity : %f\n", fHSens);
	printf("Vertical(y) sensitivity : %f\n", fVSens);
	
	same = iVSens == iHSens;
	if (same) {
		printf("1) Done! Both sensitivities same\n");
	}
	else
	{
		printf("1) Set vertical sensitivity to same as horizontal\n");
	}
	//printf("2) Remove 25 degree camera turn when you let go of aim button\n");
	printf("2) Set sensitivity\n");
	printf("3) Exit\n");
	printf("Select a number and press enter !\n");

	std::cin >> choice;
	if (!choice == 1 || !choice == 2 || !choice == 3)
	{
		printf("Incorrect input !!!\n");
		CloseHandle(ProgramHandle);
		exit(0);
	}

	switch (choice)
	{
		case 1:
			if (!same)
			{
				WriteFloat(VSens, fHSens);
			}
			break;

		case 2:
		
			printf("Current sensitivity: %f\n", fVSens);
			printf("Ingame sensitivity slider goes from 0.000312 to 0.005\n");
			printf("Enter sensitivity:\n");
			std::cin >> inputFloat;
			if (inputFloat != 0.0)
			{
				printf("Setting sensitivity to: %f\n", inputFloat);
				WriteFloat(VSens, inputFloat);
				WriteFloat(HSens, inputFloat);
				printf("Set !\n");
			}
			break;

		case 3:
			CloseHandle(ProgramHandle);
			exit(0);
			break;

		default:
			printf("Incorrect input !!!\n");
			CloseHandle(ProgramHandle);
			exit(0);
			break;
	}

	goto Start;
	return 1;
}