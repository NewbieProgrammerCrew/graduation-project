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

class Wraight: public RefCountable
{
public:
	int _hp = 150;
	int _posX = 0;
	int _posY = 0;
};

using WraightRef = TSharedPtr<Wraight>;

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
		// 중간에 개입 가능
		//target->AddRef();

	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;

		// TODO : 쫒아간다
		if (_target->_hp == 0)
		{
			//_target->ReleaseRef();
			_target = nullptr;
			return true;
		}

		return false;
	}

	WraightRef _target = nullptr;

};

using MissileRef = TSharedPtr<Missile>;

int main()
{
	
	WraightRef wraight( new Wraight());
	wraight->ReleaseRef();
	MissileRef missile( new Missile());	
	missile->ReleaseRef();

	missile->SetTarget(wraight);

	// 레이스가 피격당함
	wraight->_hp = 0;
	//delete wraight;
	//wraight->ReleaseRef();
	wraight = nullptr;

	while (true)
	{
		if (missile) {
			if (missile->Update())
			{
				//missile->ReleaseRef();
				missile = nullptr;
			}
		}
	}

	//delete missile;
	//missile->ReleaseRef();
	missile = nullptr;
}
