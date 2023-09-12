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


class Knight 
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

};

int main()
{
	// 1) 이미 만들어진 클래스 대상으로 사용 불가
	// 2) 순환 ( Cycle ) 문제

	// shared_ptr
	// weak_ptr

	// [Knight | RefCountingBlock(uses, weak)]

	//[T*][RefCountBlock*]

	// RefCountBlock(useCount(shared), weakCount)
	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	// 둘중 하나 사용
	// 1.
	bool expired = wpr.expired();

	// 2.
	shared_ptr<Knight> spr2 =  wpr.lock();
	if (spr2 != nullptr)
	{

	}
}