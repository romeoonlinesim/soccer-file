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

#include "BehaviorBlockDirection.h"
#include "VisualSystem.h"
#include "Formation.h"
#include "Dasher.h"
#include "BasicCommand.h"
#include "BehaviorPosition.h"
#include "Agent.h"
#include "PositionInfo.h"
#include "Logger.h"
#include "Evaluation.h"
#include "Geometry.h"

const BehaviorType BehaviorBlockDirectionExecuter::BEHAVIOR_TYPE = BT_Block_Direction;

namespace
{
bool ret = BehaviorExecutable::AutoRegister<BehaviorBlockDirectionExecuter>();
}

BehaviorBlockDirectionExecuter::BehaviorBlockDirectionExecuter(Agent & agent) :
	BehaviorExecuterBase<BehaviorDefenseData>(agent)
{
	Assert(ret);
}

BehaviorBlockDirectionExecuter::~BehaviorBlockDirectionExecuter(void)
{
}

bool BehaviorBlockDirectionExecuter::Execute(const ActiveBehavior & beh)
{
	Logger::instance().LogGoToPoint(mSelfState.GetPos(), beh.mTarget, "@BlockDirection");

	return Dasher::instance().GoToPoint(mAgent, beh.mTarget, beh.mBuffer, beh.mPower, true, false);
}

BehaviorBlockDirectionPlanner::BehaviorBlockDirectionPlanner(Agent & agent):
	BehaviorPlannerBase<BehaviorDefenseData>( agent)
{
}

BehaviorBlockDirectionPlanner::~BehaviorBlockDirectionPlanner()
{
}

void BehaviorBlockDirectionPlanner::Plan(std::list<ActiveBehavior> & behavior_list)
{
    Unum closest_opp_to_ball = mPositionInfo.GetClosestOpponentToBall();
    Unum closest_opp_to_tm = mPositionInfo.GetClosestOpponentToTeammate(mSelfState.GetUnum());
	Unum closest_tm = mPositionInfo.GetClosestTeammateToBall();
	// if the ball is not live (in set pieces, etc.)
	if(mWorldState.GetPlayMode() >= PM_Opp_Corner_Kick &&
			mWorldState.GetPlayMode() <=PM_Opp_Offside_Kick){
		return;
	}

	// if closest teammate is not this player OR this player's intercept cycle <= sure teammate intercept cycle, this player will block
	if (closest_tm != mSelfState.GetUnum() ) {
		//create an ActiveBehavior with its behavior set as block, then modify its parameters later
		ActiveBehavior blockDirection(mAgent, BT_Block_Direction);

        Vector ballPos = mBallState.GetPos();
		Vector currentPos = mWorldState.GetTeammate(mSelfState.GetUnum()).GetPos();
		Ray closest_opp_to_opp_dir(mWorldState.GetOpponent(closest_opp_to_tm).GetPos(), mWorldState.GetOpponent(closest_opp_to_tm).GetBodyDir());
		Vector predict_closest_opp_to_opp_pos = closest_opp_to_opp_dir.GetPoint(1);
		//Line predict_closest_opp_to_opp_line = Line(closest_opp_to_opp_dir);
		//Vector predict_closest_opp_to_opp_pos(predict_closest_opp_to_opp_line.GetX(), predict_closest_opp_to_opp_line.GetY());
		//Vector ball_to_closest_opp = (mWorldState.GetOpponent(closest_opp_to_ball).GetPos()- mWorldState.GetOpponent(closest_opp_to_tm).GetPos()).Dist();
        //AngleDeg opp_to_closest_opp = (mWorldState.GetOpponent(closest_opp_to_ball).GetPos() - mWorldState.GetOpponent(closest_opp_to_tm).GetPos()).Dir();
		//Vector opp_pass_line = mWorldState.GetOpponent(closest_opp_to_ball).GetPos() - mWorldState.GetOpponent(closest_opp_to_tm).GetPos();
        //Vector distance = mWorldState.GetTeammate(mSelfState.unum).Dist(ball_to_closest_opp);
		//Ray targetVector(mWorldState.GetTeammate(mSelfState.GetUnum()).GetPos(), opp_to_closest_opp);
		//Vector target = targetVector.GetPoint()
		//Line opp_pass_line(mWorldState.GetOpponent(closest_opp_to_ball).GetPos(), mWorldState.GetOpponent(closest_opp_to_tm).GetPos());
		Line opp_pass_line(mWorldState.GetOpponent(closest_opp_to_ball).GetPos(), predict_closest_opp_to_opp_pos);
		//Vector target = opp_pass_line.GetClosestPointInBetween(currentPos, opp_pass_line.GetProjectPoint(currentPos), mWorldState.GetOpponent(closest_opp_to_tm).GetPos());
		//Vector target = opp_pass_line.GetClosestPointInBetween(currentPos, mWorldState.GetOpponent(closest_opp_to_ball).GetPos(), mWorldState.GetOpponent(closest_opp_to_tm).GetPos());
		Vector target = opp_pass_line.GetClosestPointInBetween(currentPos, mWorldState.GetOpponent(closest_opp_to_ball).GetPos(), predict_closest_opp_to_opp_pos);

		blockDirection.mBuffer = 0.5;
		blockDirection.mPower = mSelfState.CorrectDashPowerForStamina(ServerParam::instance().maxDashPower());
		//blockDirection.mTarget = mWorldState.GetTeammate(mSelfState.GetUnum()).GetPos()  + Polar2Vector(blockDirection.mBuffer , opp_to_closest_opp);
		blockDirection.mTarget = target;
		blockDirection.mEvaluation = Evaluation::instance().EvaluatePosition(blockDirection.mTarget, true);

		// add this behavior to the behavior list
		behavior_list.push_back(blockDirection);
	}
}

