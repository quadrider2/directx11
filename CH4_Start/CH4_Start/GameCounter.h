//A class to succesfully keep track of the Game Timer
#ifndef GAMECOUNTER_H
#define GAMECOUNTER_H

#include <Windows.h>

class GameCounter {
public:
	GameCounter();
	float mGameTime()const;
	float mDeltatime()const;

	void Reset();
	void Tick();
	void Start();
	void Stop();
private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

GameCounter::GameCounter() :mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
mPausedTime(0), mStopTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	//Grabs the performance Frequency
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

//Compares the current frame with the prev frame to see
// how long it took to switch
void GameCounter::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0.0;
		return;
	}
	//Get the time for current frame
	__int64 CurrFrameTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&CurrFrameTime);
	mCurrTime = CurrFrameTime;

	//time diference between the prev frame and this one
	mDeltaTime = (mCurrTime - mPrevTime)*mSecondsPerCount;

	//Prepare the next Frame
	mPrevTime = mCurrTime;

	//Force nonnegative value, if the processor switches
	//or goes into power save mode it could return negative
	if (mDeltaTime < 0.0) 
	{
		mDeltaTime = 0.0;
	}
}

float GameCounter::mDeltatime()const
{
	return (float)mDeltaTime;
}

void GameCounter::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}
//Stop the timer because we are paused
void GameCounter::Stop()
{
	//If we are already stopped then dont do it again!!
	if (!mStopped) 
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		//Stop for the first time? Set Stop to true and save
		//the time we stopped
		mStopTime = currTime;
		mStopped = true;
	}
}
//Start the timer again making sure we dont add the paused time
void GameCounter::Start()
{
	__int64 StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&StartTime);

	if (mStopped)
	{
		//Accumulate the full pause time
		mPausedTime += (StartTime - mStopTime);

		//Since we are starting the timer again,
		//The current time is not valid, reset it to 
		//the current time;
		mPrevTime = StartTime;

		//no longer stopped
		mStopTime = 0;
		mStopped = false;
	}
}

float GameCounter::mGameTime()const
{
	//Do not count the time if we are stopped

	if (mStopped)
	{
		return (float)(((mStopTime - mPausedTime) -
			mBaseTime)*mSecondsPerCount);
	}

	//else return the time the app was running - paused time
	else
	{
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	}
}
#endif