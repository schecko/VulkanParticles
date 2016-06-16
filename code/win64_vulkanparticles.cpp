


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

struct WorkQueue
{
	uint32_t volatile nextEntryToComplete;
	uint32_t volatile entryCount;
	uint32_t volatile entryCompletionCount;
	HANDLE semaphoreHandle;
};

struct ThreadInfo
{
	int logicalThreadIndex;
	WorkQueue* workQueue;
};




WorkQueueEntry workEntries[256];



void AddWorkQueueEntry(WorkQueue* workQueue)
{
	Assert(workQueue->entryCount < 256, "too many entries");
	PreventWriteInstructionsFromCrossingThisLine //this line stops the compiler from letting the entrycount++ instruction 
	//move above this line durig optimization
	++workQueue->entryCount; //entrycount must be incremented AFTER data is placed into the work queue
	//because between the lines another thread could snatch up garbage
	//since it only checked for entry count being larger than nextentry to print
	ReleaseSemaphore(workQueue->semaphoreHandle, 1, nullptr);
}

struct WorkQueueItem
{
	bool isValid;
	uint32_t index;
};

WorkQueueItem GetNextWorkQueueItem(WorkQueue* workQueue)
{
	WorkQueueItem result;
	result.isValid = false;
	if (workQueue->nextEntryToComplete < workQueue->entryCount)
	{
		result.index = InterlockedIncrement(&workQueue->nextEntryToComplete) - 1;
		result.isValid = true;
		PreventReadInstructionsFromCrossingThisLine
	}
	return result;
}

void MarkQueueItemComplete(WorkQueue* workQueue, WorkQueueItem* item)
{
	InterlockedIncrement(&workQueue->entryCompletionCount);
}

inline bool DoThreadWork(WorkQueue* workQueue, uint32_t logicalThreadIndex)
{
	WorkQueueItem item = GetNextWorkQueueItem(workQueue);
	if(item.isValid)
	{
		//do stuff
		char buffer[256];
		WorkQueueEntry* entry = workEntries + item.index;
		wsprintf(buffer, "thread %u: %s\n", logicalThreadIndex, entry->stringToPrint);
		printf(buffer);
		MarkQueueItemComplete(workQueue, &item);
	}
	return item.isValid;
}

bool QueueWorkStillExists(WorkQueue* workQueue)
{
	return (workQueue->entryCount != workQueue->entryCompletionCount);
}

void PushString(WorkQueue* workQueue, char* str)
{
	uint32_t index = workQueue->nextEntryToComplete;
	workEntries[index].stringToPrint = str;
	AddWorkQueueEntry(workQueue);
}


DWORD WINAPI ThreadProc(LPVOID lp)
{
	ThreadInfo* threadInfo = (ThreadInfo*)lp;
	for (;;)
	{
		if(!DoThreadWork(threadInfo->workQueue, threadInfo->logicalThreadIndex))
		{
			//put thread to sleep
			WaitForSingleObjectEx(threadInfo->workQueue->semaphoreHandle, INFINITE, false);
		}

	}

	return 0;
}



int main(int argv, char** argc)
{
	MainMemory* m = (MainMemory*)MemAlloc(sizeof(MainMemory));
	uint32_t poolSize = Gibibytes(1);

	WorkQueue workQueue = {};
	ThreadInfo threadInfo[7];
	workQueue.semaphoreHandle = CreateSemaphoreEx(nullptr, 
		ArrayCount(threadInfo), 
		ArrayCount(threadInfo), 
		0, 0,
		SEMAPHORE_ALL_ACCESS);

	for (int i = 0; i < ArrayCount(threadInfo); i++)
	{
		ThreadInfo* info = threadInfo + i;
		info->logicalThreadIndex = i;
		info->workQueue = &workQueue;
		DWORD threadID;
		HANDLE threadHandle = CreateThread(nullptr, 0, ThreadProc, info, 0, &threadID);
		CloseHandle(threadHandle); //releases handle, but not the thread
	}


	PushString(&workQueue, "string 0");
	PushString(&workQueue, "string 1");
	PushString(&workQueue, "string 2");
	PushString(&workQueue, "string 3");
	PushString(&workQueue, "string 4");
	PushString(&workQueue, "string 5");
	PushString(&workQueue, "string 6");
	PushString(&workQueue, "string 7");
	PushString(&workQueue, "string 8");
	PushString(&workQueue, "string 9");
	PushString(&workQueue, "string 10");

	Sleep(1000);

	PushString(&workQueue, "string 0a");
	PushString(&workQueue, "string 1a");
	PushString(&workQueue, "string 2a");
	PushString(&workQueue, "string 3a");
	PushString(&workQueue, "string 4a");
	PushString(&workQueue, "string 5a");
	PushString(&workQueue, "string 6a");
	PushString(&workQueue, "string 7a");
	PushString(&workQueue, "string 8a");
	PushString(&workQueue, "string 9a");
	PushString(&workQueue, "string 10a");

	while (QueueWorkStillExists(&workQueue))
	{
		DoThreadWork(&workQueue, 7);
	};

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

