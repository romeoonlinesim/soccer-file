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

#include "BehaviorDribble.h"
#include "Agent.h"
#include "Kicker.h"
#include "WorldState.h"
#include "Strategy.h"
#include "Formation.h"
#include "Dasher.h"
#include "Types.h"
#include "Logger.h"
#include "VisualSystem.h"
#include "CommunicateSystem.h"
#include "Utilities.h"
#include <sstream>
#include <vector>
#include <utility>
#include "Evaluation.h"
#include <cmath>


using namespace std;

const BehaviorType BehaviorDribbleExecuter::BEHAVIOR_TYPE = BT_Dribble;

namespace {
bool ret = BehaviorExecutable::AutoRegister<BehaviorDribbleExecuter>();
}

BehaviorDribbleExecuter::BehaviorDribbleExecuter(Agent & agent) :
    BehaviorExecuterBase<BehaviorAttackData>( agent )
{
	Assert(ret);
}

BehaviorDribbleExecuter::~BehaviorDribbleExecuter(void)
{
}

bool BehaviorDribbleExecuter::Execute(const ActiveBehavior & dribble)
{
	Logger::instance().LogDribble(mBallState.GetPos(), dribble.mTarget, "@Dribble", true);

	if(dribble.mDetailType == BDT_Dribble_Normal) {
		Vector ballpos = mBallState.GetPos();
		PlayerState   oppState = mWorldState.GetOpponent(mPositionInfo.GetClosestOpponentToBall());
		Vector  	 	posOpp   = oppState.GetPos();
		ballpos = mBallState.GetPredictedPos(1);
		Vector agentpos = mSelfState.GetPos();
		Vector agent_v = agentpos - mSelfState.GetPos();
		AngleDeg agentang = mSelfState.GetBodyDir();

		AtomicAction act;


		if ( mSelfState.GetStamina() < 2700){
			Dasher::instance().GoToPoint(mAgent,act,dribble.mTarget,0.01,30);
		}
		else {
			Dasher::instance().GoToPoint(mAgent,act,dribble.mTarget, 0.01 ,100 );
		}

		act.mDashPower = MinMax(-ServerParam::instance().maxDashPower(), act.mDashPower, ServerParam::instance().maxDashPower());

		agent_v = mSelfState.GetVel() * mSelfState.GetDecay();
		if(act.mType != CT_Dash){
			agentpos = mSelfState.GetPredictedPos(1);
		}
		else
			agentpos = mSelfState.GetPredictedPosWithDash(1,act.mDashPower,act.mDashDir);

		bool collide = mSelfState.GetCollideWithPlayer();
		if ( ballpos.Dist(agentpos) > 0.95 * ( mSelfState.GetKickableArea())
				|| collide )//will not be kickable or will make a collision ??
		{
			int p = ( ( mBallState.GetPos() - mSelfState.GetPos() ).Dir() - mSelfState.GetBodyDir()) > 0 ? 1:-1 ;
			double outSpeed = mSelfState.GetVel().Mod();
			if ( act.mType == CT_Dash && fabs( act.mDashDir ) < FLOAT_EPS )
				outSpeed += mSelfState.GetAccelerationFront(act.mDashPower);
			if((agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) - mBallState.GetPos()).Mod() <
					(	agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) -mSelfState.GetPos()).Mod()) {
				if ( mSelfState.GetStamina() < 2700)
				{
					return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,30 ); // dash slow
				}
				else return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,100 );

			}
			return Kicker::instance().KickBall(mAgent ,agentpos + Polar2Vector(mSelfState.GetKickableArea(),agentang + p * 45) , outSpeed,KM_Hard);
		}
		else {
			if ( mSelfState.GetStamina() < 2700)
			{
				return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,30 ); // dash slow
			}
			else return Dasher::instance().GoToPoint( mAgent , dribble.mTarget,0.01,100 );
		}
	}
	else /*if(dribble.mDetailType == BDT_Dribble_Fast)*/{
		return Kicker::instance().KickBall(mAgent, dribble.mAngle, dribble.mKickSpeed, KM_Quick);
	}

}
BehaviorDribblePlanner::BehaviorDribblePlanner(Agent & agent) :
	BehaviorPlannerBase <BehaviorAttackData>(agent)
{
}


BehaviorDribblePlanner::~BehaviorDribblePlanner(void)
{
}


void BehaviorDribblePlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
	// if ball is not in this player's control (not in kickable area), return
	if (!mSelfState.IsKickable()) return;
	// if strategy forbids a player to dribble, return
	if (mStrategy.IsForbidenDribble()) return;
	// if player is goalie, return
	if (mSelfState.IsGoalie()) return;

	// for 180 degrees angle in front of player, evaluate dribble_NORMAL behavior and add it to the list if possible
	for (AngleDeg dir = -90.0; dir < 90.0; dir += 2.5) {
		ActiveBehavior dribble(mAgent, BT_Dribble, BDT_Dribble_Normal);

		dribble.mAngle = dir;

		// get all players that are in a specific distance with the ball
		const std::vector<Unum> & opp2ball = mPositionInfo.GetCloseOpponentToBall();
		AngleDeg min_differ = HUGE_VALUE;

		for (uint j = 0; j < opp2ball.size(); ++j) {
			// if this player's distance to ball > 15 then skip this player
			Vector rel_pos = mWorldState.GetOpponent(opp2ball[j]).GetPos() - mBallState.GetPos();
			if (rel_pos.Mod() > 15.0) continue;

			// if this player's angle difference to this direction is smaller than min_differ, modify min_differ
			AngleDeg differ = GetAngleDegDiffer(dir, rel_pos.Dir());
			if (differ < min_differ) {
				min_differ = differ;
			}
		}

		// if min_differ < 10 then skip this direction
		if (min_differ < 10.0) continue;

		// dribble target position = player's position + vector polar (probably the distance if the player runs with max speed in the current direction)
		dribble.mTarget= mSelfState.GetPos() + Polar2Vector( mSelfState.GetEffectiveSpeedMax(), dir);

		// set behavior's evaluation to the calculation of (target position)
		dribble.mEvaluation = Evaluation::instance().EvaluatePosition(dribble.mTarget, true);

		// add behavior to the list
		mActiveBehaviorList.push_back(dribble);
	}
	double speed = mSelfState.GetEffectiveSpeedMax();

	// for 180 degrees angle in front of player, evaluate dribble_FAST behavior and add it to the list if possible
	for (AngleDeg dir = -90.0; dir < 90.0; dir += 2.5) {
		ActiveBehavior dribble(mAgent, BT_Dribble, BDT_Dribble_Fast);
		dribble.mKickSpeed = speed;
		dribble.mAngle = dir;

		const std::vector<Unum> & opp2ball = mPositionInfo.GetCloseOpponentToBall();
		// calculate target point from kickspeed and current direction
		// seems like fast dribble means kick the ball to a further position than normal then run to it
		Vector target = mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * 10, dribble.mAngle);
		// if the target point is out of field the skip
		if(!ServerParam::instance().pitchRectanglar().IsWithin(target)){
			continue;
		}

		bool ok = true;
		for (uint j = 0; j < opp2ball.size(); ++j) {
			Vector rel_pos = mWorldState.GetOpponent(opp2ball[j]).GetPos() - target;
			// if distance from this opponent < kickspeed * 12 
			// OR opponent's position config (confidence?) < min valid conf
			// set ok = false and break the loop
			if (rel_pos.Mod() < dribble.mKickSpeed * 12 ||
					mWorldState.GetOpponent(opp2ball[j]).GetPosConf() < PlayerParam::instance().minValidConf()){
				ok = false;
				break;
			}
		}
		// if ok = false then skip this direction
		if(!ok){
			continue;
		}

		dribble.mEvaluation = 0;
		// add the evaluations of position within the range of 1-8 kickspeed (kick the ball with the force between 1-8)
		for (int i = 1; i <= 8; ++i) {
			dribble.mEvaluation += Evaluation::instance().EvaluatePosition(mBallState.GetPos() + Polar2Vector(dribble.mKickSpeed * i, dribble.mAngle), true);
		}
		// take the average evaluation and set target
		dribble.mEvaluation /= 8;
		dribble.mTarget = target;

		mActiveBehaviorList.push_back(dribble);
	}

	// add the behavior with highest evaluation point to the list
	if (!mActiveBehaviorList.empty()) {
		mActiveBehaviorList.sort(std::greater<ActiveBehavior>());
		behavior_list.push_back(mActiveBehaviorList.front());
	}
}
