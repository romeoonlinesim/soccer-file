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

#include "Dasher.h"
#include "Logger.h"
#include "Agent.h"
#include "Geometry.h"
#include "Kicker.h"

double Dasher::GETBALL_BUFFER = 0.1;

Array<double, 8> Dasher::DASH_DIR;
Array<int, 8> Dasher::ANTI_DIR_IDX;
Array<double, 8> Dasher::DIR_RATE;

Dasher::Dasher()
{
	Assert(std::fabs(ServerParam::instance().dashAngleStep() - 45.0) < FLOAT_EPS);

	for (int i = 0; i < 8; ++i) {
		DASH_DIR[i] = GetNormalizeAngleDeg(ServerParam::instance().dashAngleStep() * i);
		DIR_RATE[i] = GetDashDirRate(DASH_DIR[i]);
}

	for (int i = 0; i < 8; ++i) {
		ANTI_DIR_IDX[i] = GetDashDirIdx(DASH_DIR[i] + 180.0);
	}
}

//==============================================================================
Dasher::~Dasher()
{
}

//==============================================================================
Dasher & Dasher::instance()
{
	static Dasher dasher;
	return dasher;
}


/** ?????????????????????????????????
* Run to the destination point with the fastest method.
* \param agent the agent itself.
* \param act an atomic action caculated to go to pos for this cycle.
* \param pos the destination point.
* \param buffer point buffer, means the real destinantion is a cycle with the certer 
* 		of pos and radius of buffer.
* \param power the intend power to use.
* \param can_inverse true means can run in the inverse direction of the agent's body.
* \param turn_first true means that the agent should turn first to adjust the direction.
*/
void Dasher::GoToPoint(Agent & agent, AtomicAction & act, Vector target, double buffer, double power, bool can_inverse, bool turn_first)
{
	act.Clear();

	const Vector & my_pre_pos = agent.Self().GetPredictedPos(1);
	double dist = my_pre_pos.Dist(target);

	if (dist < buffer){
		act.mSucceed = true;
		return;
	}
	AngleDeg dirdiff = GetNormalizeAngleDeg((target - my_pre_pos).Dir() - agent.GetSelf().GetBodyDir());

	if (fabs(dirdiff) < 90.0){
		can_inverse = false; //??????????????????
	}

	bool inverse = false;
	if (can_inverse){
		int cycle = CycleNeedToPointWithCertainPosture(agent.Self(), target, true);
		if (cycle < 9 && cycle < CycleNeedToPointWithCertainPosture(agent.Self(), target, false)){
			inverse = true;
		}
	}

	GoToPointWithCertainPosture(agent, act, target, buffer, power, inverse, turn_first);
}

/** ???????????????????????????????????????????????????????????????
* Run to the destination point with a certain posture, forward or backword.
* \param agent the agent itself.
* \param act an atomic action caculated to go to pos for this cycle.
* \param pos the destination point.
* \param buffer point buffer, means the real destinantion is a cycle with the center
* 		of pos and radius of buffer.
* \param power the intend power to use.
* \param inverse true means run backward.
* \param turn_first true means that the agent should turn first to adjust the direction.
*/
void Dasher::GoToPointWithCertainPosture(Agent & agent, AtomicAction & act, Vector target, double buffer, double power, const bool inverse, bool turn_first)
{
	act.Clear();

	const Vector & my_pre_pos = agent.Self().GetPredictedPos(1);
	double dist = my_pre_pos.Dist(target);
	AngleDeg dirdiff = inverse? GetNormalizeAngleDeg((my_pre_pos - target).Dir() - agent.GetSelf().GetBodyDir()): GetNormalizeAngleDeg((target - my_pre_pos).Dir() - agent.GetSelf().GetBodyDir()); //?????????????????????

	double bufang;

	buffer = Max(0.05, buffer); //TODO: wait test

	if (buffer > FLOAT_EPS){
		bufang = ASin(buffer / dist / 2) * 2;
		bufang = Max(10.0, bufang);
	}
	else {
		bufang = 0.0;
	}

	if (fabs(dirdiff) > bufang){
		TurnDashPlaning(agent, act, target, buffer, power, inverse, turn_first);
	}
	else {
		power = inverse? -fabs(power): fabs(power);
		power = agent.GetSelf().CorrectDashPowerForStamina(power);
		if (fabs(power) > FLOAT_EPS){
			act.mType = CT_Dash;
			act.mDashPower = AdjustPowerForDash(agent.Self(), target, buffer, power);
			act.mDashPower = agent.GetSelf().CorrectDashPowerForStamina(act.mDashPower);
			if (fabs(act.mDashPower) > FLOAT_EPS){
				act.mSucceed = true;
			}
			else {
				act.mType = CT_None; //power???????????????????????????????????????
				TurnDashPlaning(agent, act, target, buffer, power, inverse, turn_first);//????????????????????????????????????????????????????????????
			}
		}
	}
}

/**
* ?????????????????????dash
* Planing to turn or to dash.
* @param agent the agent itself.
* @param act an atomic action to return the caculated decision.
* @param target the target position to go to.
* @param buffer
* @param power the intend power for dash.
* @param inverse true means running backwards.
* @param turn_first true means turn first manually.
*/
void Dasher::TurnDashPlaning(Agent & agent, AtomicAction & act, Vector target, double buffer, double power, bool inverse, bool turn_first)
{
	act.Clear();

	power = inverse? -fabs(power): fabs(power);
	power = agent.GetSelf().CorrectDashPowerForStamina(power);

	PlayerState & self = agent.Self();
	const Vector & my_pre_pos = self.GetPredictedPos(1);
	double target_dist = my_pre_pos.Dist(target);
	AngleDeg target_ang = inverse? GetNormalizeAngleDeg((my_pre_pos - target).Dir() - agent.GetSelf().GetBodyDir()): GetNormalizeAngleDeg((target - my_pre_pos).Dir() - agent.GetSelf().GetBodyDir()); //?????????????????????

	//DT_none
	//??????????????????,??????1dash + 1turn??????
	double bufang;
	if(buffer > FLOAT_EPS){
		bufang = ASin(buffer / target_dist / 2) * 2;
		bufang = Max(10.0, bufang);
	}
	else{
		bufang = 0.0;
	}

	const double & effort = self.GetEffort();
	const double & accrate = self.GetDashPowerRate();
	const double & inertia_moment = self.GetInertiaMoment();
	const double & decay = self.GetPlayerDecay();
	const double & speedmax = self.GetEffectiveSpeedMax();
	const Vector & vel = self.GetVel();

	const double & facing = self.GetBodyDir();
	double speed = vel.Mod();


	double diffang = fabs(target_ang);
    double oneturnang = self.GetMaxTurnAngle();
	if(buffer < FLOAT_EPS && target_dist > 2.6){
		oneturnang += 2.6;
	}

	if(turn_first || diffang <= oneturnang || speed < 0.04){ //??????????????????????????????????????????????????????????????????????????????????????????????????????,0.04??????????????????????????????
		act.mType = CT_Turn;
		act.mTurnAngle = target_ang;
		act.mSucceed = true;
		return;
	}
	else {
		//??????1 dash + 1turn;
		//???????????????dash?????????????????????
		double mspd1 = (180.0/ ( diffang + 10 ) - 1.0 ) / inertia_moment; //10???buf?????????????????????????????????????????????
		double dash2spd = Min(mspd1 / decay, speedmax) / (1 + ServerParam::instance().playerRand());

		if( fabs(GetNormalizeAngleDeg(vel.Dir() - facing)) > 90.0){
			speed = -speed;
		}

		double spd_inf = Max(speed - accrate * ServerParam::instance().maxDashPower(), -dash2spd);
		double spd_sup = Min(speed + accrate * ServerParam::instance().maxDashPower(), dash2spd);
		Ray cur_r(self.GetPos(), facing);
		Vector pt_inf = cur_r.GetPoint(spd_inf * (1.0 + decay));
		Vector pt_sup = cur_r.GetPoint(spd_sup * (1.0 + decay));
		Vector pt;//????????????
		bool bnegtive;//??????????????????????????????
		Line pdl = Line(cur_r).GetPerpendicular(target);//??????
		if(pdl.IsPointInSameSide(pt_inf, pt_sup)){
			if(fabs(GetNormalizeAngleDeg(facing-target_ang)) < 90.0){
				pt = pt_sup;
				bnegtive = (spd_sup<0);
			}
			else{
				pt = pt_inf;
				bnegtive = (spd_inf<0);
			}
		}
		else{
			double dist;
			bnegtive = !cur_r.Intersection(pdl, dist); //??????
			pt = cur_r.GetPoint(dist);
		}
		double dis = pt.Dist(target);//??????????????????point?????????,??????compare with 2 turn
		double twoturnang = oneturnang + 180.0 / (1.0 + inertia_moment * fabs(speed) * decay);
		if(twoturnang > diffang){
			const Vector & pt2 = self.GetPredictedPos(2);
			double dis2 = pt2.Dist(target);
			if(dis2 < dis || diffang > 102.6){ //??????????????????????????????????????????2turn
				act.mType = CT_Turn;
				act.mTurnAngle = target_ang;
				act.mSucceed = true;
				return;
			}
		}
		double spd_need;
		if(bnegtive){
			spd_need = -(pt - self.GetPos()).Mod() / (1.0 + decay);
		}
		else{
			spd_need = (pt - self.GetPos()).Mod() / (1.0 + decay);
		}
		//?????????dash???dash??????????????????...,?????????????????????
		double turnang_after_dash = 180.0 / (1.0 + inertia_moment * fabs(spd_need) * decay) - 5.0;
		if(GetAngleDegDiffer((target - pt).Dir(), facing) > turnang_after_dash ){
			act.mType = CT_Turn;
			act.mTurnAngle = target_ang;
			act.mSucceed = true;
			return;
		}
		double power = ( spd_need - speed ) / accrate / effort;

		power = MinMax(-ServerParam::instance().maxDashPower(), power, ServerParam::instance().maxDashPower());

		//I over
		act.mType = CT_Dash;
		act.mDashPower = AdjustPowerForDash(self, pt, FLOAT_EPS, power);
		act.mDashPower = agent.GetSelf().CorrectDashPowerForStamina(act.mDashPower);
		if(fabs(act.mDashPower) > FLOAT_EPS)
		{
			act.mSucceed = true;
		}
		else {
			act.mType = CT_Turn; //power???????????????????????????????????????
			act.mTurnAngle = 0.0;
			act.mSucceed = true;
		}
	}
}

/** ?????????????????????????????????
* Run to the destination point with the fastest method.
* \param agent the agent itself.
* \param pos the destination point.
* \param buffer point buffer, means the real destinantion is a cycle with the certer 
* 		of pos and radius of buffer.
* \param power the intend power to use.
* \param can_inverse true means can run in the inverse direction of the agent's body.
* \param turn_first true means that the agent should turn first to adjust the direction.
* \return true if the action excuted successfully.
*/
bool Dasher::GoToPoint(Agent & agent, Vector pos, double buffer, double power, bool can_inverse, bool turn_first)
{
	AtomicAction act;

	GoToPoint(agent, act, agent.GetStrategy().AdjustTargetForSetplay(pos), buffer, power, can_inverse, turn_first);
	return act.Execute(agent);
}

/**
* ?????????????????????power??????????????????power??????????????????
* ?????????????????????????????????????????????power?????????????????????
* ??????????????????????????????dash???power???????????????????????????
* dash?????????
* power ????????????????????????
* This funcition is mainly used to adjust ( usually decrease ) the dash power 
* to avoid run over the target position by using a big power. The final power 
* could be 0 to avoid an unnecessary dash.
* The sign of power is given outside the function.
* @param player the player to caculate.
* @param target the target position to go to.
* @param buffer
* @param power orignal power given.
* @return power adjusted.
*/
double Dasher::AdjustPowerForDash(const PlayerState & player, Vector target, double buffer, double power)
{
	const double & speedmax = player.GetEffectiveSpeedMax();
	const double & effort = player.GetEffort();
	const double & accrate = player.GetDashPowerRate();
	const Vector & pos = player.GetPos();
	const Vector & vel = player.GetVel();

	const double & facing = player.GetBodyDir();

	if (pos.Dist(target) > speedmax + buffer){ //???????????????????????????
		return power;
	}

	if((pos + vel).Dist(target) < buffer) { //???????????????????????????????????????
		return 0.0;
	}

	Line dash_line(Ray(pos, player.GetBodyDir()));
	Vector projective_target = dash_line.GetProjectPoint(target);
	Vector acc = projective_target - pos - vel;
	double dash_power = acc.Mod() / (effort * accrate);
	if (GetAngleDegDiffer(acc.Dir(), facing) > 90.0){
		dash_power = -dash_power;
	}

	if (power > 0){
		return MinMax(ServerParam::instance().minDashPower(), dash_power, power);
	}
	else {
		return MinMax(power, dash_power, ServerParam::instance().maxDashPower());
	}
}

/**
* ??????player??????target????????????????????????????????????dash
* ?????????????????????dash????????????
*/
/**
* player ?????? target ???????????????????????????
* This function returns the minimum cycles for a player to go to a target position.
* @param player the player to caculate.
* @param target the target position to go to.
* @param can_inverse true means consider running backwards.
* @param buffer
* @return an integer to show the minimum cycles caculated.
*/
int Dasher::CycleNeedToPoint(const PlayerState & player, Vector target, bool can_inverse, double *buf)
{
	const Vector & pos = player.GetPos();
	const Vector & vel = player.GetVel();

	const double dir = (target - pos).Dir();

	double facing;
	if (player.IsBodyDirValid()) {
		facing = player.GetBodyDir();
	}
	else if (vel.Mod() > 0.26){
		facing = vel.Dir();
	}
	else {
		facing = dir; //??????????????????
	}

	double diffang = fabs(GetNormalizeAngleDeg(dir - facing));

	if (diffang < 90.0){
		can_inverse = false; //?????????????????????
	}

	if (can_inverse){
		if (buf != 0) {
			double tmp;

			int cycle1 = CycleNeedToPointWithCertainPosture(player, target, true, buf);
			int cycle2 = CycleNeedToPointWithCertainPosture(player, target, false, &tmp);

			if (cycle2 <= cycle1) {
				*buf = tmp;
			}

			return Min(cycle1, cycle2);
		}
		else {
			int cycle1 = CycleNeedToPointWithCertainPosture(player, target, true);
			int cycle2 = CycleNeedToPointWithCertainPosture(player, target, false);
			return Min(cycle1, cycle2);
		}
	}
	else {
		return CycleNeedToPointWithCertainPosture(player, target, false, buf);
	}
}

/**
* player ?????????????????????????????????????????????????????? target ?????????????????????
* This function returns the minimum cycles for a player to go to a target position with 
* a certain posture, forward or backward.
* @param player the player to caculate.
* @param target the target position to go to.
* @param inverse true means running backwards.
* @param buffer 
* @return an integer to show the minimum cycles caculated.
*/
int Dasher::CycleNeedToPointWithCertainPosture(const PlayerState & player, Vector target, const bool inverse, double *buf)
{
	int cycle = 0; //????????????

	const double & decay = player.GetPlayerDecay();
	const double & speedmax = player.GetEffectiveSpeedMax();
	const double & stamina = player.GetStamina();
	const double & stamina_inc_max = player.GetStaminaIncMax();
	const double & dash_max = ServerParam::instance().maxDashPower();
	const Vector & pos = player.GetPos();
	const Vector & vel = player.GetVel();

	const double accrate = player.GetDashPowerRate() * player.GetEffort();
	double speed = vel.Mod();

	const Vector predict_pos_1 = pos + vel;
	const Vector predict_pos_2 = predict_pos_1 + vel * decay;
	const double dir = (target - pos).Dir();
	double dis = (target - predict_pos_1).Mod();

	const double kick_area = player.IsGoalie()? ServerParam::instance().catchAreaLength(): (player.GetKickableArea() - GETBALL_BUFFER);

	if (dis <= kick_area){
		dis = pos.Dist(target);
		if (buf) *buf = 0;
		return 1;
	}

	double facing;
	if (player.IsBodyDirValid()) {
		facing = player.GetBodyDir();
	}
	else if (speed > 0.26){
		facing = vel.Dir();
	}
	else {
		facing = dir; //??????????????????
	}

	double diffang = fabs(GetNormalizeAngleDeg(dir - facing));
	const double oneturnang = player.GetMaxTurnAngle();
	const double stamina_recovery_thr = ServerParam::instance().recoverDecThr() * ServerParam::instance().staminaMax();

	double angbuf = FLOAT_EPS;
	angbuf = ASin(kick_area / dis);
	angbuf = Max(angbuf , 15.0);

	if (inverse) {
		diffang = 180.0 - diffang;
		facing = GetNormalizeAngleDeg(180.0 + facing);
	}

	//I ????????????
	if(diffang <= angbuf){ //???????????????
		target = (target - pos).Rotate(-facing);
		dis = fabs(target.X());
		double y = fabs(target.Y());
		if(y < kick_area){
			dis -= sqrt(kick_area * kick_area - y * y);
		}
		speed *= Cos(vel.Dir() - facing); //????????????????????????
	}
	else if(diffang <= oneturnang){
		cycle += 1;
		target -= predict_pos_1;
		speed *= Cos(vel.Dir() - dir); //???????????????????????????
		speed *= decay;//????????????.????????????1?????????????????????10+?????????,?????????1turn???????????????????????????
		dis = target.Mod();
		dis -= kick_area;
	}
	else{ //?????????????????????????????????
		cycle += 2;
		target -= predict_pos_2;
		speed *= Cos(vel.Dir() - dir); //???????????????????????????
		speed *= decay * decay;//????????????.????????????1?????????????????????10+?????????,?????????1turn???????????????????????????
		dis = target.Mod();
		dis -= kick_area;
	}

	if (dis <= 0){
		if(buf != NULL){
			*buf = -dis / ( speed / decay);
			*buf = Min(*buf , 0.99);
		}
		return Max(cycle, 0);
	}

	//II ?????? & ??????????????????
	const double stamina_used_per_cycle = inverse? dash_max * 2.0: dash_max;
	const int full_cyc = int((stamina - stamina_recovery_thr) / (stamina_used_per_cycle - stamina_inc_max)); //???????????????
	int acc_cyc = 0;//????????????
	const double speedmax_thr = speedmax * decay * 0.98;
	const double accmax = accrate * dash_max;

	while(acc_cyc < full_cyc && speed < speedmax_thr){
		speed += accmax;
		if(speed > speedmax){
			speed = speedmax;
		}
		dis -= speed;
		if(dis <= 0){//?????????????????????????????????...
			cycle += acc_cyc + 1;
			if(buf != NULL){
				*buf = -dis /( speed / decay );
				*buf = Min(*buf , 0.99);
			}
			return Max(cycle, 0);
		}
		speed *= decay;
		++ acc_cyc;
	}

	cycle += acc_cyc;

	//III ??????????????????
	int aver_cyc = full_cyc - acc_cyc;
	double aver_cyc_dis = aver_cyc * speedmax;
	if(aver_cyc_dis >= dis){
		if(buf != NULL){
			double realcyc = cycle + dis / speedmax;
			cycle = int( ceil(realcyc) );
			*buf = cycle - realcyc;
			return Max(cycle, 0);

		}
		else{
			cycle = int(ceil( cycle + dis / speedmax));
			return Max(cycle, 0);
		}
	}
	else{
		cycle += aver_cyc;
		dis -= aver_cyc_dis;
	}

	//IV ??????(0??????)????????????
	double acc_tired = stamina_inc_max * accrate;
	double speed_tired = acc_tired / (1 - decay);
	double speed_tired_thr = speed_tired * decay;
	speed *= decay;
	while(dis > 0 && fabs(speed - speed_tired_thr) > 0.004){
		speed += acc_tired;
		dis -= speed;
		speed *= decay;
		++cycle;
	}

	if(dis <= 0){
		if(buf != NULL){
			*buf = -dis / ( speed / decay);
			*buf = Min(*buf , 0.99);
		}
		return Max(cycle, 0);
	}

	//V ??????(0??????)????????????

    if( buf != NULL){
        double realcyc = cycle + dis / speed_tired;
        cycle = int( ceil( realcyc ) );
        *buf = cycle - realcyc;
        return Max(cycle, 0);
    }
    else{
    	cycle = cycle + int(ceil ( dis / speed_tired));
    	return Max(cycle, 0);
    }
}

//=============================================================================
/**
* ???????????????????????????
* Turn body to a certain direction.
* @param agent the agent itself.
* @param ang the angle to turn to.
* @return an atomic action to turn the body.
*/
AtomicAction Dasher::GetTurnBodyToAngleAction(const Agent & agent, AngleDeg ang)
{
	AtomicAction action;
    action.mType = CT_Turn;

    double angle_turn = GetNormalizeAngleDeg(ang - agent.GetSelf().GetBodyDir());
    if (fabs(angle_turn) < agent.GetSelf().GetMaxTurnAngle())
    {
        action.mSucceed = true;
        action.mTurnAngle = angle_turn;
    }
    else
    {
        action.mSucceed = false;
        action.mTurnAngle = Sign(angle_turn) * agent.GetSelf().GetMaxTurnAngle();
    }
    return action;
}


/*! This method determines the optimal dash power to mantain an optimal speed
When the current speed is too high and the distance is very small, a
negative dash is performed. Otherwise the difference with the maximal speed
is determined and the dash power rate is set to compensate for this
difference.
\param posRelTo relative point to which we want to dash
\param angBody body angle of the agent
\param vel current velocity of the agent
\param dEffort current effort of the player
\param iCycles desired number of cycles to reach this point
\return dash power that should be sent with dash command */
bool Dasher::GetPowerForForwardDash(const Agent &agent, double* dash_power, Vector posRelTo, double angBody, double dEffort, int iCycles )
{
	// the distance desired is the x-direction to the relative position we
	// we want to move to. If point lies far away, we dash maximal. Furthermore
	// we subtract the x contribution of the velocity because it is not necessary
	// to dash maximal.
	if (!dash_power) {
		return false;
	}
	double dDist = posRelTo.Rotate(-angBody).X(); // get distance in direction

	if( iCycles <= 0 ) iCycles = 1;
	double dAcc  = dDist * (1 - agent.GetSelf().GetPlayerDecay()) / (1 - pow(agent.GetSelf().GetPlayerDecay(), iCycles));//get the first Geom
	// get speed to travel now
	if( dAcc > agent.GetSelf().GetEffectiveSpeedMax() )             // if too far away
	{
		dAcc = agent.GetSelf().GetEffectiveSpeedMax();                // set maximum speed
	}
	//dAcc -= vel.rotate(-angBody).x;             // subtract current velocity

	// acceleration = dash_power * dash_power_rate * effort ->
	// dash_power = acceleration / (dash_power_rate * effort )
	double dDashPower = dAcc / (agent.GetSelf().GetDashPowerRate() * dEffort );
	if( dDashPower > ServerParam::instance().maxDashPower()){
		*dash_power = ServerParam::instance().maxDashPower();
		return false;
	}
	else if( dDashPower < ServerParam::instance().minDashPower() ){
		*dash_power = ServerParam::instance().minDashPower();
		return false;
	}
	else{
		*dash_power = dDashPower;
		return true;
	}
}

/**???free???,??????int_cycle??????????????????????????????,int_cycle???-1,?????????????????????????????????
*
* \param &act
* \param /
* \return ?????????????????????,????????????????????????
*/

/**
* Caculate the get ball cycles and actions.
* @param agent the agent itself.
* @param act the atomic action to execute this cycle to get the ball.
* @param int_cycle the beginning cycle to do the action, while -1 means now.
* @param can_inverse true means allow runnning backwards to get the ball.
* @param turn_first true means the agent turn first to get the ball.
* @return a double to show the cycles needed and if it less than 0, that is to say impossible.
*/
double Dasher::GetBall(Agent & agent, AtomicAction & act, int int_cycle , bool can_inverse, bool turn_first)
{
	Assert(int_cycle == -1 || int_cycle >= 0);

	if (int_cycle == -1) {
		double min_diff = HUGE_VALUE;

		const int max_consider_cycle = Min(int(MobileState::Predictor::MAX_STEP), agent.GetStrategy().GetSureOppInterCycle() - 1);
		for (int i = 0; i <= max_consider_cycle; ++i) {
			Vector target = agent.GetWorldState().GetBall().GetPredictedPos(i);
			double my_cycle = RealCycleNeedToPoint(agent.GetSelf(), target, can_inverse);
			double diff = fabs(my_cycle - i + 1.0);

			if (diff < min_diff) {
				min_diff = diff;
				int_cycle = i;
			}
		}
	}

	if (int_cycle == -1) {
		int_cycle = Max(0, agent.GetStrategy().GetMyInterCycle());
	}

	GoToPoint(
			agent,
			act,
			agent.GetWorldState().GetBall().GetPredictedPos(int_cycle),
			agent.GetSelf().GetKickableArea() - GETBALL_BUFFER,
			agent.GetSelf().CorrectDashPowerForStamina(ServerParam::instance().maxDashPower()),
			can_inverse,
			turn_first
	);

	return RealCycleNeedToPoint(agent.GetSelf(), agent.GetWorldState().GetBall().GetPredictedPos(int_cycle), can_inverse);
}

/**
* Caculate the get ball action and execute it.
* @param agent the agent itself.
* @param int_cycle the beginning cycle to do the action, while -1 means now.
* @param can_inverse true means allow runnning backwards to get the ball.
* @param turn_first true means the agent turn first to get the ball.
* @return true means the action executed successfully.
*/
bool Dasher::GetBall(Agent & agent, int int_cycle , bool can_inverse, bool turn_first)
{
	AtomicAction act;
	GetBall(agent, act, int_cycle, can_inverse, turn_first);
	if (act.mSucceed) {
		return act.Execute(agent);
	}
	return false;
}

/*
* This function is used to correct the target position when near the ball.
* @param player the player to consider.
* @param target the target position to go to.
* @param fix the distance error to fix.
* @return position corrected.
*/
Vector Dasher::CorrectTurnForDash(const PlayerState & player, const Vector & target, double fix)
{
	if (player.GetPosConf() > 0.99 - FLOAT_EPS && player.GetBodyDirConf() > 0.99 - FLOAT_EPS) {
		Ray body_ray(player.GetPos(), player.GetBodyDir());
		Line body_line(body_ray);
		Vector closest_pt = body_line.GetProjectPoint(target);

		double buffer = 0.05 + ServerParam::instance().ballSize();
		if (closest_pt.Dist(player.GetPos()) > player.GetKickableArea() + buffer - fix){
			if (closest_pt.Dist(target)
					< player.GetKickableArea() - buffer + fix){
				return closest_pt;
			}
		}
	}
	return target;
}

/**
* ???????????????dash_power???????????????????????????
* Caculate cycles needed to a target position with a certain dash power.
* @param player the player to caculate.
* @param target the target position to go to.
* @param dash_power the power for dash to predict.
* @return an integer to show the cycles caculated.
*/
int Dasher::CyclePredictedToPoint(const PlayerState& player , Vector target , double dash_power)
{
	double corrected_dash_power = dash_power;
	double effective_power;
	double predicted_stamina = player.GetStamina();
	double predicted_effort  = player.GetEffort();
	double predicted_recovery = player.GetRecovery();
	double predicted_capacity = player.GetCapacity();
	double myang = player.GetBodyDir();
	Vector position = player.GetPos();
	Vector velocity;

	if (player.GetVelConf() < FLOAT_EPS)
	{
		velocity = Vector(0,0);
	}
	else
	{
		velocity = player.GetVel();
	}

	Vector last_position = position;
	int max_cyc = (int)(position.Dist(target) / player.GetEffectiveSpeedMax() + 100);
	int last_action = 0; /* 0 -- turn , 1 -- dash*/

	for (int i = 0;i < max_cyc;i++)
	{
		if (position.Dist(target) < PlayerParam::instance().AtPointBuffer())
		{
			return i;
		}

		if (position.Dist(target) > last_position.Dist(target) + FLOAT_EPS && last_action == 1)
		{
			return i;
		}

		last_position = position;

		/*decide if we should turn*/
		double targ_ang;
		if (dash_power > 0)
		{
			targ_ang = (target - position).Dir() - myang;
		}
		else
		{
			targ_ang = (target - position).Dir() - GetNormalizeAngleDeg(myang + 180);
		}

		double max_go_to_point_angle_err = 4; //???08CP_max_go_to_point_angle_err
		if (fabs(GetNormalizeAngleDeg(targ_ang)) > max_go_to_point_angle_err)
		{
			/*turnint*/
			double this_turn = MinMax(-player.GetMaxTurnAngle(), GetNormalizeAngleDeg(targ_ang) , player.GetMaxTurnAngle());

			myang += this_turn;
			corrected_dash_power = 0;
			last_action = 0;
		}
		else
		{
			corrected_dash_power = player.CorrectDashPowerForStamina(dash_power);
			if (fabs(corrected_dash_power) > predicted_stamina)
			{
				effective_power = Sign(corrected_dash_power) * predicted_stamina;
			}
			else
			{
				effective_power = corrected_dash_power;
			}

			effective_power *= predicted_effort;
			effective_power *= player.GetDashPowerRate();
			velocity += Polar2Vector(effective_power , myang);
			last_action = 1;
		}

		if (velocity.Mod() > player.GetEffectiveSpeedMax())
		{
			velocity *= (player.GetEffectiveSpeedMax()/ velocity.Mod());
		}

		position += velocity;
		velocity *= player.GetPlayerDecay();

		//08 ???UpdatePredictStaminaWithDash?????? ?????????????????????
		if (dash_power > 0)
		{
			predicted_stamina -= dash_power;
		}
		else
		{
			predicted_stamina -= 2 * dash_power;
		}

		if (predicted_stamina < 0)
		{
			predicted_stamina = 0;
		}

		if (predicted_stamina < ServerParam::instance().recoverDecStamina() && predicted_recovery > ServerParam::instance().recoverMin())
		{
			predicted_recovery -= ServerParam::instance().recoverDec();
		}

		if (predicted_stamina < ServerParam::instance().effortDecStamina()  && predicted_effort > player.GetEffortMin())
		{
			predicted_effort -= ServerParam::instance().effortDec();
		}

		if (predicted_stamina > ServerParam::instance().effortIncStamina() && predicted_effort < player.GetEffortMax())
		{
			predicted_effort += ServerParam::instance().effortDec();
		}

		//??????capacity??????
		double stamina_inc = Min(predicted_recovery * player.GetStaminaIncMax() , predicted_capacity);
		predicted_stamina += stamina_inc;
		if (predicted_stamina > ServerParam::instance().staminaMax())
		{
			predicted_stamina = ServerParam::instance().staminaMax();
		}

		predicted_capacity -= stamina_inc;
		if (predicted_capacity < 0)
		{
			predicted_capacity = 0;
		}
	}

	return max_cyc;
}
