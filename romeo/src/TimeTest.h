/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D)                                        *
 * BASE SOURCE CODE RELEASE 2016                                                    *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team,                   *
 *                         Multi-Agent Systems Lab.,                                *
 *                         School of Computer Science and Technology,               *
 *                         University of Science and Technology of China            *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the      *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer Simulation Team BE LIABLE    *
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL       *
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR       *
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       *
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    *
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF *
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#ifndef __TimeTest_H__
#define __TimeTest_H__


#include <vector>
#include <cstring>
#include "Utilities.h"

/**
 * ?????????????????????????????????
 * Interface to calculate the time cost of a function.
 */
#define TIMETEST(func_name) TimeTestFunc time_test_func(func_name);


/**
 * TimeCost.
 */
struct TimeCost
{
    /**
     * Constructor.
     */
    TimeCost()
    {
        mNum        = 0;
        mAveCost    = 0;
        mMaxTime    = Time(-3, 0);
        mMaxCost    = 0;
        mMinTime    = Time(-3, 0);
        mMinCost    = 1000000;
    }

	/**
	 * ????????????
	 * Number of times a function was called.
	 */
    long    mNum;

    /**
     * ????????????
     * Average cost.
     */
    double  mAveCost; //

    /**
     * ???????????????????????????
     * Cycle which the highest cost was generated at.
     */
    Time    mMaxTime;

    /**
     * ????????????
     * Highest time cost.
     */
    long    mMaxCost;

    /**
     * ???????????????????????????
     * Cycle which the lowest cost was generated at.
     */
    Time    mMinTime;

    /**
     * ????????????
     * Lowest time cost.
     */
    long    mMinCost;
};


/**
 * TimeRecord.
 */
struct TimeRecord
{
	/**
	 * Constructor.
	 */
    TimeRecord()
    {
        mBeginTime      = RealTime(0, 0);
        mCycleTimeCost  = 0;
    }

    /**
	 * ??????????????????
	 * Time cost for every cycle.
	 */
    TimeCost    mEachCycle;

    /**
     * ??????????????????
     * Time cost for each call.
     */
    TimeCost    mEachTime;

    /**
     * ????????????????????????
     * Begin time for each call.
     */
    RealTime    mBeginTime;

    /**
     * ??????????????????????????????
     * Total cost for each cycle.
     */
    long        mCycleTimeCost;
};


/**
 * TimeTest.
 */
class TimeTest
{
    TimeTest();

public:
    ~TimeTest();

    /**
     * ????????????
     * Instacne.
     */
    static TimeTest & instance();

    /**
     * ?????????????????????????????????????????????current_time???????????????????????????
     * Update when each cycle begins.
     */
    void Update(Time current_time);

    /**
     * ?????????????????????
     * Begin of each test.
     */
    int Begin(std::string func_name);

    /**
     * ?????????????????????
     * End of each test.
     */
    void End(int event_id);

    /**
     * ????????????????????????????????????????????????
     * Set self unum, which record files will be named after.
     */
    inline void SetUnum(int unum) { mUnum = unum; }

private:
    std::vector<std::string>    mEventQueue; // ???????????????????????????
    std::vector<TimeRecord>     mRecordQueue; // ?????????????????????
    std::vector<bool>           mIsExecute; // ????????????????????????????????????
    std::vector<bool>           mIsBegin; // ????????????Begin()???End()????????????

    Time        mUpdateTime; // ??????update?????????
    int         mUnum; // ???????????????
};


/**
 * TimeTestFunc
 */
class TimeTestFunc
{
public:
    TimeTestFunc(std::string func_name);
    ~TimeTestFunc();

private:
    int mEventID;
};


#endif

