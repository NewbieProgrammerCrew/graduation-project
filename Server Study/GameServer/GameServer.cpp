#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <Windows.h>
#include <future>
#include "ThreadManager.h"

#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

DECLSPEC_ALIGN(16)
class Data	// :public SListEntry
{
public:
	SLIST_ENTRY _entry;

	int64 _rand = rand() % 1000;
};

SLIST_HEADER* GHeader;

int main()
{
	GHeader = new SLIST_HEADER();
	ASSERT_CRASH(((uint64)GHeader % 16) == 0);

	::InitializeSListHead(GHeader);

	for (int32 i = 0; i < 3; ++i)
	{
		GThreadManager->Launch([]() {
			while (true)
			{		
				Data* data = new Data();
				ASSERT_CRASH(((uint64)data % 16) == 0);

				::InterlockedPushEntrySList(GHeader, (SLIST_ENTRY*)data);
				this_thread::sleep_for(10ms);
			}
		});
	}

	for (int32 i = 0; i < 2; ++i)
	{
		GThreadManager->Launch([]() {
			while (true)
			{
				Data* pop = nullptr;
				pop = (Data*)::InterlockedPopEntrySList(GHeader);

				if (pop)
				{
					cout << pop->_rand << endl;
					delete pop;
				}
				else
				{
					//cout << "NONE" << endl;
				}
			}
		});
	}

	GThreadManager->Join();
}