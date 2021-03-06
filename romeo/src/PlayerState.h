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

#ifndef __PLAYERSTATE_H__
#define __PLAYERSTATE_H__

#include "BaseState.h"
#include "PlayerParam.h"
#include "BallState.h"

class PlayerState : public MobileState
{
public:
	/**???????????????????????? ??????????????????????????????????????????????????????
	* ????????????????????????????????????????????????????????????????????????????????????????????????*/
	class ArmPoint
	{
	public:
		ArmPoint(int = 0):
			mMovableBan(0),
			mExpireBan(0),
			mTargetDist(0),
			mTargetDir(0)
		{
		}

		/**???????????????????????????????????????*/
		int mMovableBan;

		/**??????????????????????????????????????????*/
		int mExpireBan;

		/**?????????????????????*/
		double mTargetDist;

		/**?????????????????????*/
		AngleDeg mTargetDir;
	};

	/**?????????????????? ????????????????????????????????????????????????????????????????????????????????????*/
	class FocusOn
	{
	public:
		FocusOn(int = 0):
			mFocusSide('?'),
			mFocusNum(0)
		{
		}

		/**???????????????????????????*/
		char mFocusSide;

		/**??????????????????*/
		Unum mFocusNum;
	};

public:
	PlayerState();
	virtual ~PlayerState() {}

	/**????????????
	* @param ??????
	* @param ??????
	* @param ?????????*/
	void UpdateStamina(double stamina);

	/**?????????????????????????????????
	*/
	const double & GetStamina() const { return mStamina; }

	/**??????effort
	* @param ??????
	* @param ??????
	* @param ?????????*/
	void UpdateEffort(double effort);
	void UpdateCapacity(double capacity);

	/**???????????????????????????effort
	*/
	const double & GetEffort() const { return mEffort; }

	const double & GetCapacity() const { return mCapacity; }

	/**
	 * Update recovery
	 * @param recovery
	 */
	void UpdateRecovery(double recovery) { mRecovery = recovery;}

	/** get recovery*/
	const double & GetRecovery() const { return mRecovery;}

	/**??????????????????
	* @param ??????
	* @param ??????
	* @param ?????????*/
	void UpdateNeckDir(double dir , int delay = 0, double conf = 1);

	/**?????????????????????????????????????????????
	*/
	double GetNeckGlobalDir() const { return GetNormalizeAngleDeg(GetNeckDir() + GetBodyDir()); }

	/**??????????????????????????????????????????
	* ???????????????GetGlobalNeckDir??????
	*/
	double GetNeckDir() const { return GetNormalizeNeckAngle(mNeckDir.mValue); }

	/**????????????????????????????????????????????????
	*/
	int GetNeckDirDelay() const { return mNeckDir.mCycleDelay; }

	/**???????????????????????????????????????????????????
	*/
	double GetNeckDirConf() const { return mNeckDir.mConf; }

	/**??????????????????
	* @param ??????
	* @param ??????
	* @param ?????????*/
	void UpdateBodyDir(double dir , int delay = 0, double conf = 1);

	/**???????????????????????????????????????
	*/
    const double & GetBodyDir() const { return mBodyDir.mValue; }

	/**????????????????????????????????????????????????
	*/
	int GetBodyDirDelay() const { return mBodyDir.mCycleDelay; }

	/**???????????????????????????????????????????????????
	*/
	double GetBodyDirConf() const { return mBodyDir.mConf; }

	void UpdateTackleBan(int ban);

	int GetTackleBan() const { return mTackleBan; }

	/**????????????????????????
	* @param ??????????????????????????????
	*/
	void UpdateCatchBan(int ban);

	/**???????????????????????????????????????????????? ???????????????*/
	int GetCatchBan() const { return mCatchBan; }

	/**?????????????????????
	* @param ?????????
	* @param ??????
	* @param ?????????
	* @param ??????
	* @param ??????????????????
	* @param ??????????????????*/
	void UpdateArmPoint(AngleDeg dir , int delay = 0, double conf = 1 , double  dist = 0 , int move_ban = -1 , int expire_ban = -1);

	/**????????????*/
	bool IsPointing() const {return mIsPointing;}
	void SetIsPointing(bool is_pointing) {mIsPointing = is_pointing;}

	/**????????????????????????*/
	AngleDeg GetArmPointDir() const { return GetNormalizeAngleDeg(mArmPoint.mValue.mTargetDir); }

	/**???????????????????????? ????????????????????????*/
	double  GetArmPointDist() const { return mArmPoint.mValue.mTargetDist; }

	/**???????????????????????????????????? ????????????????????????*/
	int GetArmPointMovableBan() const { return mArmPoint.mValue.mMovableBan; }

	/**????????????????????????????????? ????????????????????????*/
	int GetArmPointExpireBan() const { return mArmPoint.mValue.mExpireBan; }

	/**??????????????????????????????*/
	int GetArmPointDelay() const { return mArmPoint.mCycleDelay; }

	/**???????????????????????????*/
	double GetArmPointConf() const { return mArmPoint.mConf; }

	/**??????????????????
	* @param ????????????
	* @param ???????????????
	* @param ??????
	* @param ?????????*/
	void UpdateFocusOn(char side , Unum num , int delay = 0 , double conf = 1);

	/**???????????????????????????*/
	char GetFocusOnSide() const { return mFocusOn.mValue.mFocusSide; }

	/**???????????????????????????*/
	Unum GetFocusOnUnum() const { return mFocusOn.mValue.mFocusNum; }

	/**???????????????????????????*/
	int GetFocusOnDelay() const { return mFocusOn.mCycleDelay; }

	/**????????????????????????*/
	double GetFocusOnConf() const { return mFocusOn.mConf; }

	/**???????????????????????????*/
	void UpdateKicked(bool is_kicked);

	/**???????????????????????????*/ //  ***********************?????????????????????**********************
	void UpdateTackling(bool is_tackling);

	/**??????????????????????????????*/
	void UpdateLying(bool is_lying);

	/**??????????????????????????? ??????????????????????????????*/
	bool IsKicked() const { return mIsKicked; }

	/**??????????????????????????? ??????????????????????????????*/
	bool IsAlive() const { return mIsAlive; }

	void SetIsAlive(bool alive);

	/**??????????????????????????????????????????tackle_ban???*/
	bool IsTackling() const { return mTackleBan > 0; }

	bool IsLying() const { return mFoulChargedCycle > 0; }

	/**????????????????????????*/
	CardType GetCardType() const { return mCardType; }
	void UpdateCardType(CardType card_type) { if ( mCardType == CR_None || ( mCardType == CR_Yellow && card_type == CR_Red ) ) mCardType = card_type; }

	/**?????????????????????????????????*/
	int GetFoulChargedCycle () const { return mFoulChargedCycle; }

	void UpdateFoulChargedCycle(int x)
	{
		mFoulChargedCycle = x;
	}

	bool IsIdling() const {
		Assert(!(IsTackling() && IsLying()));
		return IsTackling() || IsLying();
	}

	int GetIdleCycle() const {
		Assert(!(IsTackling() && IsLying()));
		return GetTackleBan() + GetFoulChargedCycle();
	}

	/**?????????????????????*/
	int GetPlayerType() const { return mPlayerType; }

	/**?????????????????????*/
	void UpdatePlayerType(int type);

	/**???????????????????????????*/
	void UpdateViewWidth(ViewWidth width);

	/**??????????????????*/
	ViewWidth GetViewWidth() const { return mViewWidth; }

	/**get view angle*/
	AngleDeg GetViewAngle() const { return sight::ViewAngle(mViewWidth);}

	/**???????????????
	* @param ?????????delay???????????????
	* @param ?????????conf???????????????*/
	void AutoUpdate(int delay_add = 1 , double conf_dec_factor = 1);

	/**??????????????????Unum*/
	void UpdateUnum(Unum num);

	/**??????Unum*/
	Unum GetUnum() const { return mUnum; }

	/**update collide with ball*/
	void UpdateCollideWithBall(bool collide) { mCollideWithBall = collide;}

	/**get collide with ball*/
	bool GetCollideWithBall() const { return mCollideWithBall;}

	/**update collide with player*/
	void UpdateCollideWithPlayer(bool collide) { mCollideWithPlayer = collide;}

	/** get collide with player*/
	bool GetCollideWithPlayer() const { return mCollideWithPlayer;}

	/** update collide with post*/
	void UpdateCollideWithPost(bool collide) { mCollideWithPost = collide;}

	/** get collide with post*/
	bool GetCollideWithPost() const { return mCollideWithPost;}

	/**?????????????????????????????????PlayerState*/
	void GetReverseFrom(const PlayerState & o);

private:
	/**???????????????????????????*/
	double mStamina;

	/**??????Effort*/
	double mEffort;

	/**??????capacity*/
	double mCapacity;

	/**store Recovery*/
	double mRecovery;

	/**?????????????????????*/
	StateValue<double> mNeckDir;

	/**?????????????????????*/
	StateValue<double> mBodyDir;

	/**??????????????????????????????*/
	int mTackleBan;

	/**?????????????????????????????? ???????????????*/
	int mCatchBan;

	/**??????????????????*/
	StateValue<ArmPoint> mArmPoint;

	/**??????????????????*/
	StateValue<FocusOn>  mFocusOn;

	/**???????????????????????????*/
	bool mIsPointing;

	bool mIsKicked;

	/**???????????? ???????????????????????????*/
	bool mIsAlive;

	/**????????????*/
	int mPlayerType;

	/**????????????*/
	ViewWidth mViewWidth;

	/**
	 * ?????????Unum
	 * + ??????????????????- ???????????????
	 * */
	Unum mUnum;

	/**
	 * whether collide with ball
	 */
	bool mCollideWithBall;

	/**
	 * whether collide with player
	 */
	bool mCollideWithPlayer;

	/**
	 * whether collide with post
	 */
	bool mCollideWithPost;

	int mFoulChargedCycle; //????????????????????????

	//==============================================================================
public:
	bool IsKickable() const { return mIsKickable; }
	bool IsKickable(const BallState &, const double & buffer) const;
	void UpdateKickable(bool kickable) { mIsKickable = kickable; }

	double GetKickRate() const { return mKickRate; }
	void UpdateKickRate(double kick_rate) { mKickRate = kick_rate; }

	bool IsBallCatchable() const { return mIsGoalie && mBallCatchable; }
	void UpdateBallCatchable(bool ball_catchable) { mBallCatchable = ball_catchable; }

	bool UnderDangerousTackleCondition() const { return mUnderDangerousTackleCondition; }
	void UpdateDangerousTackleCondition(const bool may) { mUnderDangerousTackleCondition = may; }

	double GetTackleProb(const bool foul) const { return mTackleProb[foul]; }
	void UpdateTackleProb(double tackle_prob, const bool foul = false) { mTackleProb[foul] = tackle_prob; }

	AngleDeg GetMaxTurnAngle() const { return mMaxTurnAngle; }
	void UpdateMaxTurnAngle(AngleDeg max_turn_angle) { mMaxTurnAngle = max_turn_angle; }

	bool IsTired() const {return mIsTired;}
	void UpdateTired(bool is_tired) { mIsTired = is_tired;}

    const double & GetMinStamina() const { return mMinStamina; }
    void UpdateMinStamina(const double min_stamina) { mMinStamina = min_stamina; } 

	bool IsBodyDirMayChanged() const { return mIsBodyDirMayChanged;}
	void UpdateBodyDirMayChanged(bool may_changed) { mIsBodyDirMayChanged = may_changed;}

	//????????????Pos_delay  ?????????BT_Intercept??????;
	void ResetPosDelay(int delay)
	{
		Assert(delay >= 0);
		double conf = 1;
		for (int i = 0;i < delay;i++)
		{
			conf  *= PlayerParam::instance().playerConfDecay();
		}

		UpdatePos(GetPos() , delay , conf);
	}

private:
	bool        mIsKickable;  /** kick */
	double      mKickRate;
	bool        mBallCatchable; /** catch */

	Array<double, 2,true> mTackleProb;    /** tackle */
	bool mUnderDangerousTackleCondition; //??????????????????????????????????????????

	AngleDeg    mMaxTurnAngle;  /** turn */
	bool		mIsTired;		/** tell is  tired or not*/
    double      mMinStamina; // ???????????????????????????player???extraStamina
	CardType    mCardType;  /** card */

	//just for update body dir other should not use
	bool        mIsBodyDirMayChanged;

public:
	bool IsGoalie() const { return mIsGoalie; }
	void UpdateIsGoalie(bool is_goalie) { mIsGoalie = is_goalie; }

private:
	bool        mIsGoalie;

public:
    bool IsSensed() const { return mIsSensed; }
    void UpdateIsSensed(bool is_sensed) { mIsSensed = is_sensed; }

private:
    bool        mIsSensed; // ???true?????????????????????sense??????????????????????????????????????????

public:
	/** some useful interfaces */
	const double & GetPlayerSpeedMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerSpeedMax(); }
	const double & GetStaminaIncMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).staminaIncMax(); }
	const double & GetPlayerDecay() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay(); }
	const double & GetInertiaMoment() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).inertiaMoment(); }
	const double & GetDashPowerRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).dashPowerRate(); }
	const double & GetPlayerSize() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).playerSize(); }
	const double & GetKickableMargin() const {return PlayerParam::instance().HeteroPlayer(mPlayerType).kickableMargin(); }
	const double & GetKickRand() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickRand(); }
	const double & GetKickPowerRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickPowerRate(); }
	const double & GetExtraStamina() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).extraStamina(); }
	const double & GetEffortMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).effortMax(); }
	const double & GetEffortMin() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).effortMin(); }
	virtual const double & GetKickableArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).kickableArea(); }
	const double & GetCatchAreaLStretch() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).catchableAreaLStretch(); }
	const double & GetMinCatchArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).minCatchArea(); }
	const double & GetMaxCatchArea() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).maxCatchArea(); }
	const double & GetFoulDetectProbability() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).foulDetectProbability(); }

	double GetAccelerationRateOnDir(const double dir) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationRateOnDir(dir); }
    const double & GetAccelerationFrontRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFrontRate(); }
    const double & GetAccelerationSideRate() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSideRate(); }
    const double & GetAccelerationFrontMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFrontMax(); }
    const double & GetAccelerationSideMax() const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSideMax(); }
    const double & GetAccelerationFront(const double & power) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationFront(power); }
    const double & GetAccelerationSide(const double & power) const { return PlayerParam::instance().HeteroPlayer(mPlayerType).accelerationSide(power); }

    AngleDeg GetRandAngle(const double & power ,const double & vel ,const BallState & bs ) const;
	double GetCatchProb( const double & dist ) const;
	double GetControlBallProb(const Vector & ball_pos) const; //??????kick???catch???tackle??????????????????????????????

	Vector GetPredictedPosWithDash(int steps = 1, double dash_power = 0, AngleDeg dash_dir = 0.0) const;
	Vector GetPredictedVelWithDash(int steps = 1, double dash_power = 0, AngleDeg dash_dir = 0.0) const;

	double CorrectDashPowerForStamina(double dash_power) const;

	AngleDeg GetEffectiveTurn(AngleDeg moment, double my_speed) const;
	AngleDeg GetEffectiveTurn(AngleDeg moment) const
	{ return GetEffectiveTurn(moment, GetVel().Mod()); }
	AngleDeg GetMaxEffectiveTurn(double my_speed) const /* how much we'll actually turn if we try max turn */
	{ return GetEffectiveTurn(ServerParam::instance().maxMoment(), my_speed); }

public:
	bool IsBodyDirValid() const { return GetBodyDirConf() > 0.991; }

	bool IsOutOfStamina() const { return (GetStamina() + GetCapacity() < PlayerParam::instance().MinStamina() + FLOAT_EPS); }

};

//make to be inlined
inline void PlayerState::UpdateStamina(double stamina)
{
	mStamina = stamina;
}

inline void PlayerState::UpdateEffort(double effort)
{
	mEffort = effort;
}

inline void PlayerState::UpdateCapacity(double capacity)
{
	mCapacity = capacity;
}

inline void PlayerState::UpdateNeckDir(double dir, int delay, double conf)
{
	mNeckDir.mValue = GetNormalizeAngleDeg(dir);
	mNeckDir.mCycleDelay = delay;
	mNeckDir.mConf  = conf;
}

inline void PlayerState::UpdateBodyDir(double dir, int delay, double conf)
{
	mBodyDir.mValue = GetNormalizeAngleDeg(dir);
	mBodyDir.mCycleDelay = delay;
	mBodyDir.mConf = conf;
}

inline void PlayerState::UpdateTackleBan(int ban)
{
	mTackleBan = ban;
}

inline void PlayerState::UpdateCatchBan(int ban)
{
	mCatchBan = ban;
}

inline void PlayerState::UpdateArmPoint(AngleDeg dir, int delay, double conf, double dist, int move_ban , int expire_ban )
{
	ArmPoint arm;
	arm.mTargetDir = GetNormalizeAngleDeg(dir);
	arm.mTargetDist = dist;
	arm.mMovableBan = move_ban;
	arm.mExpireBan  = expire_ban;

	mArmPoint.mValue = arm;
	mArmPoint.mCycleDelay = delay;
	mArmPoint.mConf  = conf;
}

inline void PlayerState::UpdateFocusOn(char side, Unum num, int delay, double conf)
{
	FocusOn focus;
	focus.mFocusSide = side;
	focus.mFocusNum  = num;

	mFocusOn.mValue = focus;
	mFocusOn.mCycleDelay = delay;
	mFocusOn.mConf  = conf;
}

inline void PlayerState::UpdateKicked(bool is_kicked)
{
	mIsKicked = is_kicked;
}

inline void PlayerState::AutoUpdate(int delay_add, double conf_dec_factor)
{
	MobileState::AutoUpdate(delay_add , conf_dec_factor);

	mNeckDir.AutoUpdate(delay_add , conf_dec_factor);
	mBodyDir.AutoUpdate(delay_add , conf_dec_factor);
	mArmPoint.AutoUpdate(delay_add , conf_dec_factor);
	mFocusOn.AutoUpdate(delay_add , conf_dec_factor);

	mCollideWithPost = false;
	mCollideWithPlayer = false;
	mCollideWithBall = false;
	mIsKicked    = false;
	mIsTired     = false;

	if (mTackleBan > 0) {
		mTackleBan -= delay_add;
		mTackleBan = Max(mTackleBan, 0);
	}

	if (mFoulChargedCycle > 0) {
		mFoulChargedCycle -= delay_add;
		mFoulChargedCycle = Max(mFoulChargedCycle, 0);
	}

	if (mCatchBan > 0) {
		mCatchBan -= delay_add;
		mCatchBan = Max(mCatchBan, 0);
	}

	if (mArmPoint.mValue.mExpireBan > 0) {
		mArmPoint.mValue.mExpireBan -= delay_add;
		mArmPoint.mValue.mExpireBan = Max(mArmPoint.mValue.mExpireBan, 0);
	}

	if (mArmPoint.mValue.mMovableBan > 0) {
		mArmPoint.mValue.mMovableBan -= delay_add;
		mArmPoint.mValue.mMovableBan = Max(mArmPoint.mValue.mMovableBan, 0);
	}

	if (GetPosDelay() > delay_add) {
		mIsBodyDirMayChanged = true;
	}
}

inline void PlayerState::UpdatePlayerType(int type)
{
	if (mPlayerType != type) {
		mPlayerType = type;

		SetDecay(PlayerParam::instance().HeteroPlayer(mPlayerType).playerDecay());
		SetEffectiveSpeedMax(PlayerParam::instance().HeteroPlayer(mPlayerType).effectiveSpeedMax());
	}
}

inline void PlayerState::UpdateViewWidth(ViewWidth width)
{
	mViewWidth = width;
}

inline void PlayerState::UpdateUnum(Unum num)
{
	mUnum = num;
}

inline void PlayerState::SetIsAlive(bool alive)
{
	mIsAlive = alive;

	if (!mIsAlive) {
		UpdatePos(GetPos(), GetPosDelay(), 0.0);
		UpdateVel(GetVel(), GetVelDelay(), 0.0);

		mNeckDir.mConf = 0.0;
		mBodyDir.mConf = 0.0;
	}
}

#endif
