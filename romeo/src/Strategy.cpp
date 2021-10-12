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

#include <vector>
#include "Strategy.h"
#include "Formation.h"
#include "InfoState.h"
#include "PositionInfo.h"
#include "InterceptInfo.h"
#include "Tackler.h"
#include "Dasher.h"
#include "Logger.h"
#include "Utilities.h"
using namespace std;

//==============================================================================
Strategy::Strategy(Agent & agent):
	DecisionData(agent)
{
    mSituation = ST_Defense;
    mLastController = 0;

    mIsBallActuralKickable = false;
    mIsBallFree = false;
    mController = 0;
    mChallenger = 0;

    mForbiddenDribble = false;

    mIsPenaltyFirstStep = false;
    mPenaltyTaker = 0;
    mPenaltySetupTime = 0;
}

Strategy::~Strategy()
{
}

bool Strategy::IsMyControl() const
{
	return mController == mAgent.GetSelfUnum();
}

bool Strategy::IsLastMyControl() const
{
    return mLastController == mAgent.GetSelfUnum();
}

bool Strategy::IsTmKickable() const
{
	return (mInfoState.GetPositionInfo().GetTeammateWithBall() > 0);
}

// this function is called each routine in class Updatable in Utilities.h?
void Strategy::UpdateRoutine()
{
	StrategyAnalyze();
	PenaltyAnalyze();
}

void Strategy::StrategyAnalyze()
{
    mIsLastBallFree = mIsBallFree;
    if (mIsBallFree == false)//记录上次不free时的状态，参考WE2008
    {
        mLastController = mController;
		mLastChallenger = mChallenger;
    }

	mIsBallActuralKickable = false;
	mIsBallFree = true;
	mController = 0;

	mMyInterCycle = 150;
	mMinTmInterCycle = 150;
	mMinOppInterCycle = 150;
	mMinIntercCycle = 150;
	mSureTmInterCycle = 150;
	mSureOppInterCycle = 150;
	mSureInterCycle = 150;
	mFastestTm = 0;
	mFastestOpp = 0;
	mSureTm = 0;
	mSureOpp = 0;

	mBallInterPos = mWorldState.GetBall().GetPos();
	mBallFreeCycleLeft = 0;
	mBallOutCycle = 1000;

	mController = 0;
	mChallenger = 0;

	if (mForbiddenDribble) {
		if (!mSelfState.IsKickable()) {
			mForbiddenDribble = false;
		}
	}

	// analyze ball first
	BallPossessionAnalyse();

	// analyze situation
	SituationAnalyse();

    //And once they are over, roll back formation. Formation will be updated by BehaviorData when needed.
    mFormation.Rollback("Strategy");

	if (mSelfState.GetTackleProb(false) < FLOAT_EPS) {
		if ( mIsBallFree) {
			if (mMyTackleInfo.mMinCycle >= mSureInterCycle || (mController > 0 && mController != mSelfState.GetUnum())) {
				mMyTackleInfo.mRes = IR_Failure;
			}
		}
	}
	else {
		mMyTackleInfo.mRes = IR_Success;
		mMyTackleInfo.mMinCycle = 0;
	}
}

/**
 * 球权分析
 */
void Strategy::BallPossessionAnalyse()
{
	// get vector of int that contains player that are close to ball (?)
	const std::vector<Unum> & p2b = mInfoState.GetPositionInfo().GetClosePlayerToBall();
	// get player's self state
	const PlayerState & self = mAgent.GetSelf();
	// get ball state
	BallState & ball = mAgent.World().Ball();

	//假设ball_free，拦截计算
	// assuming ball_free and process intercept calculation
	mIsBallFree = true;
	// if ball is in field?
	if (ServerParam::instance().pitchRectanglar().IsWithin(ball.GetPos())){
		// initialize Ray? (Ray like Raycasting which points from the ball position which points to ball velocity direction)
		Ray ballcourse(ball.GetPos(), ball.GetVel().Dir());
		Vector outpos; //(0,0)
		// find which quarter of the field the ball is?
		if (ServerParam::instance().pitchRectanglar().Intersection(ballcourse, outpos)){
			double distance = outpos.Dist(ball.GetPos());
			mBallOutCycle = (int)ServerParam::instance().GetBallCycle(ball.GetVel().Mod(), distance);
		}
        else
        {
            mBallOutCycle = 0; // 可能没有交点，比如ball位置的x坐标正好为52.5
        }
	}
	else {
		mBallOutCycle = 0;
	}
	mController = self.GetUnum();

	//分析谁拿球
	//这里对场上所有其他队员计算拦截时都考虑了cyc_delay,得到的拦截周期是最小拦截周期,
	//认为自己该去拿球的情况是,最快的对手不比自己快太多,最快的队友是自己(暂时不考虑buf)

	//Analyze who gets the ball
	//Here, cyc_delay is considered when calculating interceptions for all other players on the field, and the interception period obtained is the minimum interception period.
	//The situation that I think I should get the ball is that the fastest opponent is not much faster than myself, and the fastest teammate is myself (not considering buf for the time being) 

	// ordered intercept list (sorted intercept list)?
	const std::vector<OrderedIT> & OIT = mInfoState.GetInterceptInfo().GetOIT();
	std::vector<OrderedIT>::const_iterator it, itr_NULL = OIT.end(), pMyInfo = itr_NULL, pTmInfo = itr_NULL, pOppInfo = itr_NULL;

	mBallFreeCycleLeft = 150;

	for (it = OIT.begin(); it != OIT.end(); ++it){
		// if it is opponent's player
		if (it->mUnum < 0){
			// if it is opponent's goalkeeper then skip?
			if (it->mUnum == -mWorldState.GetOpponentGoalieUnum()) continue; //这里认为对方守门员不会去抢球，否则截球里面就不去抢打身后的球了
			// if its min cycle to interception < 150
			if (it->mpInterceptInfo->mMinCycle < mMinOppInterCycle){
				// if its mCycleDelay < 16 then set fastest opponent to this player (what is mCycleDelay?)
				if (it->mCycleDelay < 16){
					mMinOppInterCycle = it->mpInterceptInfo->mMinCycle;
					mBallFreeCycleLeft = Min(mBallFreeCycleLeft, int(mMinOppInterCycle + it->mCycleDelay * 0.5)); //mOppMinInterCycle + it->cd * 0.5是对截球周期的一个权衡的估计
					mFastestOpp = -it->mUnum;
				}
			}
			// if its [min cycle + cycle delay] < [current sure opponent intercycle]
			if(it->mpInterceptInfo->mMinCycle + it->mCycleDelay < mSureOppInterCycle){
				// set current sure opponent intercycle to new one
				mSureOppInterCycle = int(it->mpInterceptInfo->mMinCycle + it->mCycleDelay);
				// mark the sure opponent to current player
				mSureOpp = -it->mUnum;
				pOppInfo = it;
				// if unable to estimate its body direction
				if (!mWorldState.GetOpponent(mSureOpp).IsBodyDirValid() && mWorldState.GetOpponent(mSureOpp).GetVel().Mod() < 0.26){ //无法估计身体方向
					mSureOppInterCycle += 1;
				}
			}
		}
		// if it is teammate's player
		else {
			// if it is the current player then set current player's parameter, set pMyInfo to player
			if (it->mUnum == self.GetUnum()){
				mMyInterCycle = it->mpInterceptInfo->mMinCycle;
				mBallFreeCycleLeft = Min(mBallFreeCycleLeft, mMyInterCycle);
				pMyInfo = it;
			}
			// if it is not the current player
			else {
				// set fastest teammate if fulfilled condition
				if (it->mpInterceptInfo->mMinCycle < mMinTmInterCycle){
					mMinTmInterCycle = it->mpInterceptInfo->mMinCycle;
					mBallFreeCycleLeft = Min(mBallFreeCycleLeft, int(mMinTmInterCycle + it->mCycleDelay * 0.5));
					mFastestTm = it->mUnum;
				}
				// set pmTmInfo to current player
				if(it->mpInterceptInfo->mMinCycle + it->mCycleDelay < mSureTmInterCycle){
					mSureTmInterCycle = int(it->mpInterceptInfo->mMinCycle + it->mCycleDelay);
					mSureTm = it->mUnum;
					pTmInfo = it;
					if (!mWorldState.GetTeammate(mSureTm).IsBodyDirValid() && mWorldState.GetTeammate(mSureTm).GetVel().Mod() < 0.26){ //无法估计身体方向
						mSureTmInterCycle += 1;
					}
				}
			}
		}
	}

	// set [sure intercept cycle] = min( sure opponent cycle, sure teammate cycle, my cycle )
	mSureInterCycle = Min(mSureOppInterCycle, mSureTmInterCycle);
	mSureInterCycle = Min(mSureInterCycle, mMyInterCycle);

	// set [min intercept cycle] = min (min opponent cycle, min teammate cycle, my cycle)
	mMinIntercCycle = Min(mMinOppInterCycle, mMinTmInterCycle);
	mMinIntercCycle = Min(mMyInterCycle, mMinIntercCycle);

	// if there is no sure teammate can intercept then set all intercept cycle to current player's intercept cycle
	if (mSureTm == 0) {
		mSureTm = mSelfState.GetUnum();
		mSureTmInterCycle = mMyInterCycle;
		mMinTmInterCycle = mMyInterCycle;
		mFastestTm = mSureTm;
	}

	// if current player's last behavior is dribble and last behavior's detail is dribble_fast 
	// and [last ball is not free] OR [my intercept cycle < sure intercept cycle + 6] OR [current time - last ball free time < 8 (ball free < 8 cycle?)]
	
	// basically if current player is running with the ball?
	if (mAgent.IsLastActiveBehaviorInActOf(BT_Dribble) && mAgent.GetLastActiveBehaviorInAct()->mDetailType == BDT_Dribble_Fast &&
			( !mIsLastBallFree || mMyInterCycle < mSureInterCycle + 6 || mWorldState.CurrentTime() - mLastBallFreeTime < 8)){
		// set ball controller = current player's number
		mController = mSelfState.GetUnum();
		// if pMyInfo is already changed in the code run above
		if (pMyInfo != itr_NULL){
			mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
		}
	}
	// if current player CAN (not 100%?) intercept ball fastest
	else if( /*mMyInterCycle < mBallOutCycle + 2 &&*/ mMyInterCycle <= mSureInterCycle ){ //自己是最快截到球的人
		// set ball controller to current player
		mController = self.GetUnum();
		// if pTmInfo is already changed from the code run above and its cycle delay < 3
		if(pTmInfo != itr_NULL && pTmInfo->mCycleDelay < 3){//2004_10
			// if the current player is controlling the ball
			if (IsMyControl()) {
				// if teammate sure cycle <= my cycle <= SURE opponent cycle
				if (mSureTmInterCycle <= mMyInterCycle && mMyInterCycle <= mSureOppInterCycle) {
					// if sure teammate's velocity delay or position delay = 0
					// AND distance between sure teammate to current player < visible distance - 0.5 (visible distance = distance that players can see?)
					if (mWorldState.GetTeammate(mSureTm).GetVelDelay() == 0 || (mWorldState.GetTeammate(mSureTm).GetPosDelay() == 0 && mInfoState.GetPositionInfo().GetPlayerDistToPlayer(mSureTm, mSelfState.GetUnum()) < ServerParam::instance().visibleDistance() - 0.5)) {
						// ball intercept position = predicted position from sure teammate intercept cycle
						Vector ball_int_pos = mBallState.GetPredictedPos(mSureTmInterCycle);
						// get position of sure teammate
						Vector pos = mWorldState.GetTeammate(mSureTm).GetPos();
						// if distance between [predicted ball position to teammate position] < [teammate's kickable area - getball_buffer?] then set controller to teammate
						if (pos.Dist(ball_int_pos) < mWorldState.GetTeammate(mSureTm).GetKickableArea() - Dasher::GETBALL_BUFFER) {
							mController = mSureTm;
						}
						else {
							// calculate speed of teammate
							Vector vel = mWorldState.GetTeammate(mSureTm).GetVel();
							double speed = vel.Mod() * Cos(vel.Dir() - (ball_int_pos - pos).Dir());
							// if teammate's speed > [max speed of that player * that player's decay? * 0.9]
							// teammate intercepting ball?
							// set controller to teammate
							if (speed > mWorldState.GetTeammate(mSureTm).GetEffectiveSpeedMax() * mWorldState.GetTeammate(mSureTm).GetDecay() * 0.9) { //队友正在截球
								mController = mSureTm;
							}
						}
					}
				}
				// if teammate sure cycle <= my cycle <= MIN opponent cycle set controller to sure teammate
				else if (mSureTmInterCycle < mMyInterCycle && mMyInterCycle <= mMinOppInterCycle) {
					mController = mSureTm;
				}
			}
		}

		// if controller is current player then calculate predicted ball interception position from current player's intercept cycle
		if (mController == mSelfState.GetUnum()) {
			if (pMyInfo != itr_NULL){
				mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
			}
            else if (mWorldState.CurrentTime().T() > 0) {
				PRINT_ERROR("bug here?");
			}
		}
		// else set to sure teammate
		else {
			mBallInterPos = ball.GetPredictedPos(mSureTmInterCycle);
			mController = mSureTm;
		}
	}
	// if player cant get the ball, see teammates' conditions
	else {//自己拿不到球了,看看队友如何
		// if sure teammate is available then set controller to teammate
		if(pTmInfo != itr_NULL && mSureTmInterCycle <= mSureInterCycle){//有可能拿到
			mBallInterPos = ball.GetPredictedPos(mSureTmInterCycle);
			mController = mSureTm;
		}
		// else if there are no teammates available
		else {
			// if opponent is available, set controller to opponent
			if(pOppInfo != itr_NULL){
				mBallInterPos = ball.GetPredictedPos(mSureOppInterCycle);
				mController = -mSureOpp;
			}
			else {
				// if ball is out of field? (maybe no one is controlling the ball) then it is free, controller = 0
				if (mMinIntercCycle > mBallOutCycle + 5) { //because ball will be out of field
					mBallInterPos = ball.GetPredictedPos(mBallOutCycle);
					mController = 0;
				}
				// else set controller to current player???
				else {
					mController = mSelfState.GetUnum();
					mBallInterPos = ball.GetPredictedPos(mMyInterCycle);
				}
			}
		}
	}

	// set kickable player to teammate or opponent or 0
	int kickable_player = mInfoState.GetPositionInfo().GetTeammateWithBall(); //这里没有考虑buffer
	if (kickable_player == 0){
		kickable_player = -mInfoState.GetPositionInfo().GetOpponentWithBall(); //这里没有考虑buffer
	}

	//if there is an available kickable player then set controller to that player
	if (kickable_player != 0) {
		mController = kickable_player;
		mIsBallFree = false;
		mBallOutCycle = 1000;
	}

	//下面判断可踢情况
	//先判断能踢到球的队员
	//过程:看能踢到球的自己人有几个,多于一个按规则决定是谁踢,得到是否自己可以踢球,存为_pMem->ball_kickable
	//规则:球离谁基本战位点谁踢

	//The following judges the possible kicking situation
	//First judge the player who can get the ball
	//Process: See how many people can play the ball, more than one will decide who is playing according to the rules, and get whether they can play the ball, save it as _pMem->ball_kickable
	//Rules: Who kicks the ball from the base position 

	// if current player is kickabl
    if (self.IsKickable()){
		mController = self.GetUnum();
		mIsBallFree = false;
		mSureInterCycle = 0;
		mMinIntercCycle = 0;
		mMyInterCycle = 0;
		mMinTmInterCycle = 0;
		mSureTmInterCycle = 0;
		mBallInterPos = ball.GetPos();
		mBallFreeCycleLeft = 0;
		mIsBallActuralKickable = true;
		// ball challenger, in this case maybe the opponent with the ball
		mChallenger = mInfoState.GetPositionInfo().GetOpponentWithBall();

		// if current player is not goalkeeper
		if (!mSelfState.IsGoalie()) {
			// square of distance between player's position in formation to the ball?
			double self_pt_dis = mAgent.GetFormation().GetTeammateFormationPoint(self.GetUnum(), ball.GetPos()).Dist2(ball.GetPos());

			// for loop players near the ball
			for(unsigned int i = 0; i < p2b.size(); ++i){
				Unum unum = p2b[i];
				// if teammate and not current player
				if(unum > 0 && unum != self.GetUnum()){
					// if teammate kickable
					if (mWorldState.GetPlayer(unum).IsKickable()){
						// if in play on mode and teammate is goalie
						if(mWorldState.GetPlayMode() != PM_Play_On && mWorldState.GetPlayer(unum).IsGoalie()/*&& unum == PlayerParam::instance().ourGoalieUnum()*/){
							// if ball is in control of goalkeeper, set self's kickable to false
							mAgent.Self().UpdateKickable(false); //非playon时如果守门员可踢把自己强行设置成不可踢
							mController = unum;
							break;
						}
						double tm_pt_dis = mAgent.GetFormation().GetTeammateFormationPoint(unum, ball.GetPos()).Dist2(ball.GetPos());
						// if teammate distance < self distance, set self kickable to false and break
						if(tm_pt_dis < self_pt_dis){
							mAgent.Self().UpdateKickable(false);
							mController = unum;
							break;
						}
					}
					// if other teammates cannot kick
					else { //可以认为其他人踢不到了
						break;
					}
				}
			}
		}
	}
	// if current player is not kickable and there are others that are kickable
	else if (kickable_player != 0 && kickable_player != self.GetUnum()){ //自己踢不到球,但有人可以
		mIsBallFree = false;
		// if teammate is kickable then set challenger to opponent with ball?
		if (kickable_player > 0){ //自己人可踢
			mChallenger = mInfoState.GetPositionInfo().GetOpponentWithBall();
		}
	}

	// call set play analyse for final analysis?
	SetPlayAnalyse(); //最后分析，作为修正
}

// return value wont be used, this function is used only to set the ball free as well as change controller if needed 
bool Strategy::SetPlayAnalyse()
{
	PlayMode play_mode = mWorldState.GetPlayMode();
	// before kick off mode, ball is always free, set controller to closest player to ball
	if (play_mode == PM_Before_Kick_Off){
		mIsBallFree = true;
		mController = mInfoState.GetPositionInfo().GetClosestPlayerToBall();
		return true;
	}
	// if 1 < mode < 21 (Our mode), ball is free, if ball has speed then it is served
	else if (play_mode < PM_Our_Mode && play_mode > PM_Play_On){
		mIsBallFree = true;
		if (mBallState.GetVel().Mod() < 0.16) { //如果有球速，说明已经发球了
			mController = mInfoState.GetPositionInfo().GetClosestTeammateToBall();
		}
		return true;
	}
	// if mode > 36 (opponent mode), ball is free, if ball has speed then it is served
	else if (play_mode > PM_Opp_Mode){
		mIsBallFree = true;
		if (mBallState.GetVel().Mod() < 0.16) { //如果有球速，说明已经发球了
			mController = -mInfoState.GetPositionInfo().GetClosestOpponentToBall();
		}
		return true;
	}
	// if other modes (not our or opponent), return false
	return false;
}

void Strategy::SituationAnalyse()
{
	// if controller is opponent and ball X position < -10 (the ball is on our half field?), set situation to defense
	if( mController < 0 && mWorldState.GetBall().GetPos().X() < -10){
		mSituation = ST_Defense;
	}
	else {
		// if ball is not free
		if(!mIsBallFree){
			// if controller is our teammate or no one
			if(mController >= 0){
				// if ball intercept position < 32 (the ball is not in opponent's 16m50 box)
				if(mBallInterPos.X() < 32.0){
					// if offside line > 40 (inside 16m50 box) AND the controller line type is midfielder AND ball intercept position > 25 (from half of opponent field to their goal) then set situation to penalty attack (why?)
					if(mInfoState.GetPositionInfo().GetTeammateOffsideLine() > 40.0 && mAgent.GetFormation().GetTeammateRoleType(mController).mLineType == LT_Midfielder && mBallInterPos.X() > 25.0){
						mSituation = ST_Penalty_Attack;
					}
					// else set situation to forward attack (maybe means direct attack)
					else {
						mSituation = ST_Forward_Attack;
					}
				}
				// if ball intercept position is in opponent's 16m50 box
				else {
					mSituation=ST_Penalty_Attack;
				}
			}
			// if controller is opponent, set situtation to defense
			else {
				mSituation = ST_Defense;
			}
		}
		// if ball is free (QUITE SIMILAR TO ABOVE IF)
		else{
			// if the ball is in this player's control OR surely teammate can intercept faster than opponent
			if(IsMyControl() || mSureTmInterCycle <= mSureOppInterCycle){
				// if ball intercept X position < 32 AND controller is our teammate
				if(mBallInterPos.X() < 32.0 && mController > 0) {
					if(mInfoState.GetPositionInfo().GetTeammateOffsideLine() > 40.0 && mAgent.GetFormation().GetTeammateRoleType(mController).mLineType == LT_Midfielder && mBallInterPos.X() > 25.0) {
						mSituation = ST_Penalty_Attack;
					}
					else {
						mSituation = ST_Forward_Attack;
					}
				}
				else {
					mSituation=ST_Penalty_Attack;
				}
			}
			// if none of teammate can intercept the ball faster than opponent, set situation to defense,
			else{
				mSituation = ST_Defense;
			}
		}
	}

	mFormation.Update(Formation::Offensive, "Strategy");
}

/**
 * Situation based strategy position
 * \param t unum of teammate.
 * \param ballpos
 * \param normal true normal situation, and position should be adjusted with offside line,
 *               false setplay, return SBSP directly.
 */
Vector Strategy::GetTeammateSBSPPosition(Unum t,const Vector& ballpos)
{
	Vector position;

	// set position of player with number t
	// if controller is our teammate OR no one is controlling the ball and the ball intercept position is on opponent's half
	if (mController > 0 ||
        (mController == 0 && mBallInterPos.X() > 10.0))
    {
		position = mAgent.GetFormation().GetTeammateFormationPoint(t, mController, ballpos);
	}
    else
    {
        position = mAgent.GetFormation().GetTeammateFormationPoint(t);
	}

	// min of [player's X position] AND [teammate offside line - at point buffer? (at_point_buffer = 1, but what is it?)]
	// this is to avoid offside
	double x = Min(position.X(),
			mInfoState.GetPositionInfo().GetTeammateOffsideLine() - PlayerParam::instance().AtPointBuffer());
	// if this player is defender, then set X position between min middle point of field and current player
	if (mAgent.GetFormation().GetTeammateRoleType(t).mLineType==LT_Defender){		//后卫不过中场，便于回防
		position.SetX(Min(0.0,x));
	}
	// else if player is forward (striker), set X position between max of -1.0 and current player's position
	// basically forward will always on the opponent half
	else if (mAgent.GetFormation().GetTeammateRoleType(t).mLineType== LT_Forward){		//前锋不回场，便于进攻…………
		position.SetX(Max( - 1.0,x));
	}
	// else if player is goalie or midfielder, keep the current position
	else position.SetX(x);

	// return player's position
	return position;
}

// return my intercept position (predict)
Vector Strategy::GetMyInterPos()
{
	return mBallState.GetPredictedPos(mMyInterCycle);
}

// set pieces? (corners, free kick, etc.), no need to modify this function
Vector Strategy::AdjustTargetForSetplay(Vector target)
{
	if (mWorldState.GetPlayMode() > PM_Opp_Mode) {
		while (target.Dist(mBallState.GetPos()) < ServerParam::instance().offsideKickMargin() + 0.5) {
			target.SetX(target.X() - 0.5);
		}

		if (mWorldState.GetPlayMode() == PM_Opp_Kick_Off) {
			target.SetX(Min(target.X(), -0.1));
		}
		else if (mWorldState.GetPlayMode() == PM_Opp_Offside_Kick) {
			target.SetX(Min(target.X(), mBallState.GetPos().X() - 0.5));
		}
		else if (mWorldState.GetPlayMode() == PM_Opp_Goal_Kick) {
			if (ServerParam::instance().oppPenaltyArea().IsWithin(target)) {
				if (mSelfState.GetPos().X() < ServerParam::instance().oppPenaltyArea().Left()) {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						target = ServerParam::instance().oppPenaltyArea().TopLeftCorner();
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						target = ServerParam::instance().oppPenaltyArea().BottomLeftCorner();
					}
					else {
						target.SetX(Min(target.X(), ServerParam::instance().oppPenaltyArea().Left() - 0.5));
					}
				}
				else {
					if (mSelfState.GetPos().Y() < ServerParam::instance().oppPenaltyArea().Top()) {
						target.SetY(Min(target.Y(), ServerParam::instance().oppPenaltyArea().Top() - 0.5));
					}
					else if (mSelfState.GetPos().Y() > ServerParam::instance().oppPenaltyArea().Bottom()) {
						target.SetY(Max(target.Y(), ServerParam::instance().oppPenaltyArea().Bottom() + 0.5));
					}
					else {
						target = mSelfState.GetPos(); //do nothing
					}
				}
			}
		}
	}

	return target;
}

// NO NEED TO MODIFY THE FUNCTIONS ABOVE SINCE THERE ARE NO REASON TO IMPROVE PENALTY TAKING

// check if our penalty has been taken
bool Strategy::IsMyPenaltyTaken() const
{
	return (mWorldState.GetPlayMode() == PM_Our_Penalty_Taken) &&
	       (mAgent.GetSelfUnum() == mPenaltyTaker);
}

// penalty check
bool Strategy::IsPenaltyPlayMode() const
{
    const PlayMode &play_mode = mWorldState.GetPlayMode();
    return (play_mode == PM_Penalty_On_Our_Field) || (play_mode == PM_Penalty_On_Opp_Field) ||
           (play_mode >= PM_Our_Penalty_Setup && play_mode <= PM_Our_Penalty_Miss) ||
           (play_mode >= PM_Opp_Penalty_Setup && play_mode <= PM_Opp_Penalty_Miss);
}

// analyze penalty situation
void Strategy::PenaltyAnalyze()
{
    if (IsPenaltyPlayMode() == false)
    {
        return;
    }

    if (mWorldState.GetPlayModeTime() == mWorldState.CurrentTime())
    {
		// penalty takers' turns?
    	static Unum penalty_taker_seq[] = {1, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2};
        const PlayMode &play_mode = mWorldState.GetPlayMode();
        if (play_mode == PM_Our_Penalty_Setup)
        {
            while (penalty_taker_seq[++mPenaltySetupTime%TEAMSIZE] == mWorldState.GetTeammateGoalieUnum());
            mPenaltyTaker = penalty_taker_seq[mPenaltySetupTime%TEAMSIZE];
        }
        else if (play_mode == PM_Opp_Penalty_Setup)
        {
            mPenaltyTaker = -1;
        }
    }
}

// end of file Strategyinfo.cpp
