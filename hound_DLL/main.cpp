#include <Windows.h>
#include <memory.h>
#include <string>

struct SharedData
{
	HINSTANCE instance = nullptr;
	LPDWORD init_fxn = nullptr;
	DWORD init_offset = 0;
};

#define DLL_INIT_FXN_NAME "DLLInit"
#define DLL_STATUS_DISPLAY_FXN_NAME "WriteStatusMessage"
#define SHMEMSIZE sizeof(SharedData)
#define SHMEMNAME L"Global\\hound_DLL_file_map"
#define LOG_FILE L"C:\\Users\\ap\\Documents\\Projects\\logs\\hound_DLL.log"

static HANDLE map_object = NULL;
static LPVOID mem_map = NULL;

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD reason, LPVOID reserved)
{
	BOOL init;
	SharedData data;

	switch (reason) {
	case DLL_PROCESS_ATTACH:
		map_object = CreateFileMappingW(INVALID_HANDLE_VALUE,
			nullptr,
			PAGE_READWRITE,
			0,
			SHMEMSIZE,
			SHMEMNAME);
		if (map_object == nullptr) return FALSE;

		init = (GetLastError() != ERROR_ALREADY_EXISTS);

		mem_map = MapViewOfFile(map_object,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			SHMEMSIZE);
		if (mem_map == nullptr) return FALSE;

		if (init) {
			memset(mem_map, 0, SHMEMSIZE);
			data.instance = hDLL;
			
			//set struct fxn ref values
			data.init_fxn = LPDWORD(GetProcAddress(hDLL, "DLLInit"));
			data.init_offset = DWORD(data.init_fxn) - DWORD(data.instance);
			memcpy_s(mem_map, SHMEMSIZE, &data, SHMEMSIZE);
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnmapViewOfFile(mem_map);
		CloseHandle(map_object);
		break;
	default:
		break;
	}


	return TRUE;
	UNREFERENCED_PARAMETER(hDLL);
	UNREFERENCED_PARAMETER(reserved);
}

void DLLInit() {}