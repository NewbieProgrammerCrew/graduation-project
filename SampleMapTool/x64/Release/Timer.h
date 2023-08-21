#pragma once
#include "stdafx.h"
const ULONG MAX_SAMPLE_COUNT = 50;
class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Start(){}
	void Stop(){}
	void Reset();
	void Tick(float fLockFPS = 0.0f); // 타이머 시간 갱신
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0); // 프레임 레이트 반환
	float GetTimeElapsed(); //프레임의 평균 경과 시간을 반환한다.
private:
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;
	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;
	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	float m_fFPSTimeElapsed;
	bool m_bStopped;


};

