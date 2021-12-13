#include "framework.h"
#include "Timer.h"

TimeManager::TimeManager()
	: mCurTime(std::chrono::system_clock::now()), mPreTime(mCurTime), mDeltaTime(0.0),
	mCurFrameLate(0), mPreFrameLate(0), mFrameTime(0)
{

}

TimeManager::~TimeManager()
{

}

double TimeManager::GetDeltaTime()
{
	mDeltaTime = std::chrono::duration<double>(mCurTime - mPreTime).count();

	return mDeltaTime;
}

void TimeManager::SetCurTime()
{
	mCurTime = std::chrono::system_clock::now();
}

void TimeManager::SetPreTime()
{
	mPreTime = std::chrono::system_clock::now();
}

int TimeManager::GetFrameLate()
{
	mFrameTime += mDeltaTime;
	if (mFrameTime > 1)
	{
		mFrameTime = 0;
		mPreFrameLate = mCurFrameLate;
		mCurFrameLate = 0;
		//printf("%d\n", mPreFrameLate);
	}

	return mPreFrameLate;
}