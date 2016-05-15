


#include "win64_vulkanparticles.h"

#define GLM_FORCE_RADIANS
#include <windows.h>
#include "commonwindows.h"
#include "util.h"
#include "vulkanparticles.h"

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

void* MemAlloc(uint32_t memorySize)
{
	void* memory = VirtualAlloc(nullptr, memorySize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	Assert(memory, "could not allocate memory");
	return memory;
}

void MemFree(void* memory)
{
	VirtualFree(memory, 0, MEM_RELEASE);
}


void PollEvents(const WindowInfo* windowInfo)
{
    MSG msg;
#if 1
    while (PeekMessage(&msg, windowInfo->windowHandle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
	for (int i = 0; i < 20; i++)
	{	
		if (!PeekMessage(&msg, windInfo->windowHandle, 0, 0, PM_REMOVE))
		{
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#endif
}

int main(int argv, char** argc)
{
	MainMemory* m = (MainMemory*)MemAlloc(sizeof(MainMemory) + Gigabytes(1));
    Init(m);
    while (m->input.running)
    {
		Tick(&m->timerInfo);
        PollEvents(&m->windowInfo);
		Update(m);
        Render(&m->deviceInfo, &m->swapchainInfo);
		Tock(&m->timerInfo);
#if DEBUGGING | VALIDATION_MESSAGES | VALIDATION_LAYERS
		Sleep(&m->timerInfo, 15);
#else
		Sleep(&m->timerInfo, 60);
#endif
    }
    Quit(m);
	MemFree(m);
    return 0;
}

