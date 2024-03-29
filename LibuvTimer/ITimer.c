#include "ITimer.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "uv.h"

struct ITimer
{
	uv_timer_t _Timer;
	TimerProc _TimerProc;
	void* _pData;
	int _RepeatCount;
};

static struct ITimer* AllocTimer(TimerProc oTimerProc, void* pData, int nRepeatCount)
{
    struct ITimer* pTimer = (struct ITimer*)malloc(sizeof(struct ITimer));
	memset(pTimer, 0, sizeof(struct ITimer));

	uv_timer_init(uv_default_loop(), &pTimer->_Timer);
	pTimer->_TimerProc = oTimerProc;
	pTimer->_pData = pData;
	pTimer->_RepeatCount = nRepeatCount;

	return pTimer;
}

static void FreeTimer(struct ITimer* pTimer)
{
	free(pTimer);
}

static void On_timer_cb(uv_timer_t* handle)
{
    struct ITimer* pTimer = (struct ITimer*)handle->data;

	if (pTimer->_RepeatCount < 0)
	{
		pTimer->_TimerProc(pTimer->_pData);
	}
	else if (pTimer->_RepeatCount == 0)
	{
		CancelTimer(pTimer);
	}
	else
	{
		--pTimer->_RepeatCount;
		pTimer->_TimerProc(pTimer->_pData);
	}
}

struct ITimer* Schedule(TimerProc oTimerProc, void* pData, int nLoopTime, int nRepeatCount)
{
    struct ITimer* pTimer = AllocTimer(oTimerProc, pData, nRepeatCount);

	pTimer->_Timer.data = pTimer;
	uv_timer_start(&pTimer->_Timer, On_timer_cb, nLoopTime, nLoopTime);

	return pTimer;
}

void CancelTimer(struct ITimer* pTimer)
{
	uv_timer_stop(&pTimer->_Timer);
	FreeTimer(pTimer);
}