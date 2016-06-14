


#include "win64_vulkanparticles.h"

#define GLM_FORCE_RADIANS
#include <windows.h>
#include <intrin.h>
#include <Strsafe.h>
#include "commonwindows.h"
#include "util.h"
#include "vulkanparticles.h"



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

struct WorkQueueEntry
{
	char* stringToPrint;
};

static uint32_t volatile nextEntryToPrint;
static uint32_t volatile entryCount;
static uint32_t volatile entryCompletionCount;
WorkQueueEntry workEntries[256];

#define PreventWriteInstructionsFromCrossingThisLine _WriteBarrier(); /*_mm_sfence(); might or might not be necessary for x86*/
#define PreventReadInstructionsFromCrossingThisLine _ReadBarrier();

void PushString(HANDLE semaphoreHandle, char* str)
{
	Assert(entryCount < 256, "too many entries");
	WorkQueueEntry* entry = workEntries + entryCount;
	entry->stringToPrint = str;
	PreventWriteInstructionsFromCrossingThisLine //this line stops the compiler from letting the entrycount++ instruction 
	//move above this line durig optimization
	entryCount++; //entrycount must be incremented AFTER data is placed into the work queue
	//because between the lines another thread could snatch up garbage
	//since it only checked for entry count being larger than nextentry to print
	ReleaseSemaphore(semaphoreHandle, 1, nullptr);
}

struct ThreadInfo
{
	HANDLE semaphoreHandle;
	int logicalThreadIndex;
};

DWORD WINAPI ThreadProc(LPVOID lp)
{
	ThreadInfo* threadInfo = (ThreadInfo*)lp;
	for (;;)
	{
		if(nextEntryToPrint < entryCount)
		{
			uint32_t entryIndex = InterlockedIncrement(&nextEntryToPrint) - 1;

			PreventReadInstructionsFromCrossingThisLine
			WorkQueueEntry* entry = workEntries + entryIndex;
			Assert(entry->stringToPrint != "", "empty string printed");
			char buffer[256];
#if 0
			StringCbPrintf(buffer, 256, "thread %u: %s\n", threadInfo->logicalThreadIndex, entry->stringToPrint);
#else
			wsprintf(buffer, "thread %u: %s\n", threadInfo->logicalThreadIndex, entry->stringToPrint);
#endif

			printf(buffer);
			InterlockedIncrement(&entryCompletionCount);
			//entry->stringToPrint = "";
		} else
		{
			//put thread to sleep
			WaitForSingleObjectEx(threadInfo->semaphoreHandle, INFINITE, false);
		}

	}

	return 0;
}

int main(int argv, char** argc)
{
	MainMemory* m = (MainMemory*)MemAlloc(sizeof(MainMemory));
	uint32_t poolSize = Gibibytes(1);


	ThreadInfo threadInfo[8];
	HANDLE semaphoreHandle = CreateSemaphoreEx(nullptr, 
		ArrayCount(threadInfo), 
		ArrayCount(threadInfo), 
		0, 0,
		SEMAPHORE_ALL_ACCESS);

	for (int i = 0; i < ArrayCount(threadInfo); i++)
	{
		ThreadInfo* info = threadInfo + i;
		info->logicalThreadIndex = i;
		info->semaphoreHandle = semaphoreHandle;
		DWORD threadID;
		HANDLE threadHandle = CreateThread(nullptr, 0, ThreadProc, info, 0, &threadID);
		CloseHandle(threadHandle); //releases handle, but not the thread
	}


	PushString(semaphoreHandle, "string 0");
	PushString(semaphoreHandle, "string 1");
	PushString(semaphoreHandle, "string 2");
	PushString(semaphoreHandle, "string 3");
	PushString(semaphoreHandle, "string 4");
	PushString(semaphoreHandle, "string 5");
	PushString(semaphoreHandle, "string 6");
	PushString(semaphoreHandle, "string 7");
	PushString(semaphoreHandle, "string 8");
	PushString(semaphoreHandle, "string 9");
	PushString(semaphoreHandle, "string 10");

	Sleep(1000);

	PushString(semaphoreHandle, "string 0a");
	PushString(semaphoreHandle, "string 1a");
	PushString(semaphoreHandle, "string 2a");
	PushString(semaphoreHandle, "string 3a");
	PushString(semaphoreHandle, "string 4a");
	PushString(semaphoreHandle, "string 5a");
	PushString(semaphoreHandle, "string 6a");
	PushString(semaphoreHandle, "string 7a");
	PushString(semaphoreHandle, "string 8a");
	PushString(semaphoreHandle, "string 9a");
	PushString(semaphoreHandle, "string 10a");

	while (entryCount != entryCompletionCount);

    Init(m, MemAlloc(poolSize), poolSize);
    while (m->input.running)
    {
		Tick(&m->timerInfo);
        PollEvents(&m->windowInfo);
		Update(m);
        Render(&m->deviceInfo, &m->swapchainInfo);
		Tock(&m->timerInfo);
#if DEBUGGING || VALIDATION_MESSAGES || VALIDATION_LAYERS
		Sleep(&m->timerInfo, 15);
#else
		Sleep(&m->timerInfo, 60);
#endif
    }
    Quit(m);
	MemFree(m);
    return 0;
}

