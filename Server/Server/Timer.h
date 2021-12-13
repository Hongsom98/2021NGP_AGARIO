#pragma once

class TimeManager
{
public:
	int mCurFrameLate;
public:
	TimeManager();
	~TimeManager();

	double GetDeltaTime();
	void SetCurTime();
	void SetPreTime();
	int GetFrameLate();

private:
	int mPreFrameLate;
	double mFrameTime;
	double mDeltaTime;
	std::chrono::system_clock::time_point mCurTime;
	std::chrono::system_clock::time_point mPreTime;
};
