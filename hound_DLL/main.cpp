#include <Windows.h>
#include <memory.h>

#define SHMEMSIZE 4096

static LPVOID mem_map = NULL;
static HANDLE map_object = NULL;

BOOL WINAPI DllMain(HINSTANCE hDLL, DWORD reason, LPVOID reserved)
{
	BOOL init, ignore;

	switch (reason) {
	case DLL_PROCESS_ATTACH:

		map_object = CreateFileMapping(INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			SHMEMSIZE,
			TEXT("hound_DLL_file_map"));
		if (map_object == NULL) return FALSE;

		init = (GetLastError() != ERROR_ALREADY_EXISTS);

		mem_map = MapViewOfFile(map_object,
			FILE_MAP_ALL_ACCESS,
			0,
			0,
			0);
		if (mem_map == NULL) return FALSE;

		if (init) memset(mem_map, '\0', SHMEMSIZE);

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:

		ignore = UnmapViewOfFile(mem_map);
		ignore = CloseHandle(map_object);

		break;
	default:
		break;
	}


	return TRUE;
	UNREFERENCED_PARAMETER(hDLL);
	UNREFERENCED_PARAMETER(reserved);
}

//DLL functions
