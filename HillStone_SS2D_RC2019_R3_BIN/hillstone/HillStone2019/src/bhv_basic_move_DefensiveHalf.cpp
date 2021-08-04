// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_move_DefensiveHalf.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicMove_DefensiveHalf::execute( PlayerAgent * agent )
{
    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove" );

    //-----------------------------------------------
    // tackle
    if ( Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )
    {
        return true;
    }

    const WorldModel & wm = agent->world();
    /*--------------------------------------------------------*/
    // chase ball
    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    if ( ! wm.existKickableTeammate()
         && ( self_min <= 3
              || ( self_min <= mate_min
                   && self_min < opp_min + 3 )
              )
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        Body_Intercept().execute( agent );
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;
    }

//------------------------------------------------------------------------
    const PlayerPtrCont & teammates = wm.teammatesFromBall();
    const PlayerPtrCont::const_iterator t_end = teammates.end();

	const PlayerObject * M_fastest_teammate;
	const PlayerObject * M_second_teammate;
	int M_teammate_reach_cycle;
	int M_second_teammate_reach_cycle;

    if ( wm.existKickableTeammate() )

    {
        dlog.addText( Logger::INTERCEPT,
                      "exist bhv_basic_move" );

 M_teammate_reach_cycle = 0;

        for ( PlayerPtrCont::const_iterator t = teammates.begin();
              t != t_end;
              ++t )
        {
            if ( (*t)->isGhost()
                   || (*t)->posCount() > wm.ball().posCount() + 1 )
            {
                continue;
            }

            M_fastest_teammate = *t;
            dlog.addText( Logger::INTERCEPT,
                          "---> set bhv_basic_move %d (%.1f %.1f)",
                          (*t)->unum(),
                          (*t)->pos().x, (*t)->pos().y );
            break;
        }

//-----------------------------------------------------------------------
    if ( wm.existKickableTeammate() )

    {
        dlog.addText( Logger::INTERCEPT,
                      "exist bhv_basic_move2" );

 M_teammate_reach_cycle = 500;

        for ( PlayerPtrCont::const_iterator t = teammates.begin();
              t != t_end;
              ++t )
        {
            if ( (*t)->isGhost()
                   || (*t)->posCount() > wm.ball().posCount() + 1 )
            {
                continue;
            }

            M_second_teammate = *t;
            dlog.addText( Logger::INTERCEPT,
                          "---> set bhv_basic_move %d (%.1f %.1f)",
                          (*t)->unum(),
                          (*t)->pos().x, (*t)->pos().y );
            break;
        }
}
//------------------------------------------------------------------------

   /*if ( M_fastest_teammate )
    {
        dlog.addText( Logger::INTERCEPT,
                      "<-----Intercept Teammate  bhv_basic_move = %d."
                      " teammate %d (%.1f %.1f)",
                      M_teammate_reach_cycle,
                      M_fastest_teammate->unum(),
                      M_fastest_teammate->pos().x,
                      M_fastest_teammate->pos().y );

if (M_fastest_teammate || (M_second_teammate->unum() == 10 || M_second_teammate->unum() == 11 || M_second_teammate->unum() == 8 || M_second_teammate->unum() == 9 && M_second_teammate->pos().x,M_second_teammate->pos().y))
	{
		 dlog.addText( Logger::TEAM,
                  __FILE__": 1_2pass_1" );

		Body_Pass();
			

	}

    }

    if ( M_second_teammate )
    {
        dlog.addText( Logger::INTERCEPT,
                      "<-----Intercept Teammate 2nd bhv_basic_move= %d."
                      " teammate %d (%.1f %.1f)",
                      M_second_teammate_reach_cycle,
                      M_second_teammate->unum(),
                      M_second_teammate->pos().x,
                      M_second_teammate->pos().y );

if (M_second_teammate || (M_fastest_teammate->unum() == 10 || M_fastest_teammate->unum() == 11 || M_fastest_teammate->unum() == 8 || M_fastest_teammate->unum() == 9 && M_fastest_teammate->pos().x,M_fastest_teammate->pos().y))
	{
		 dlog.addText( Logger::TEAM,
                  __FILE__": 1_2pass_2" );

		Body_Pass();
	}
    }*/
}
//-----------------------------------------------------------------------------


    const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    const double dash_power = Bhv_BasicMove_DefensiveHalf::getDashPower( agent );//Strategy::get_normal_dash_power( wm );

    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
    }

    if ( wm.existKickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new Neck_TurnToBall() );
    }
    else
    {
        agent->setNeckAction( new Neck_TurnToBallOrScan() );
    }


const BallObject & ball = agent->world().ball();
      Vector2D center( -40.0, 0.0 );
    if ( ball.pos().x < -45.0 ) {
       if(wm.self().unum() == 6 && center.y > -25  ){
        center.x = std::max( -40.0, ball.pos().x + 2.0 );

         dlog.addText( Logger::TEAM,
                  __FILE__": center_brock1",
                  target_point.x, target_point.y,
                  dist_thr );

      }
     else if(wm.self().unum() == 7 && center.y < 25  ){
        center.x = std::max( -40.0, ball.pos().x + 2.0 );

         dlog.addText( Logger::TEAM,
                  __FILE__": center_brock1",
                  target_point.x, target_point.y,
                  dist_thr );

      }
    } 

    Line2D block_line( wm.ball().pos(),( center - wm.ball().pos() ).th() );
    
    Vector2D block_point;
    block_point.y = std::max( 15.0, ball.pos().absY() + 10.0 );

    if ( ball.pos().y < 0.0 ){
      if(wm.self().unum() == 6 && center.y < 25  ){
        block_point.y *= -1.0;

        block_point.x = block_line.getX( block_point.y );
        
        dlog.addText( Logger::TEAM,
                  __FILE__": center_brock2",
                  target_point.x, target_point.y,
                  dist_thr );
      }
     else if(wm.self().unum() == 7 && center.y > -25  ){
        block_point.y *= -1.0;

        block_point.x = block_line.getX( block_point.y );
        
        dlog.addText( Logger::TEAM,
                  __FILE__": center_brock2",
                  target_point.x, target_point.y,
                  dist_thr );
      }
     }

    const PlayerObject * getMarkTarget( const WorldModel & wm,const Vector2D & home_pos ) ;
    double dist_opp_to_home = 50.0;
    Vector2D M_home_pos = Strategy::i().getPosition( wm.self().unum() );
    const PlayerObject * opp = wm.getOpponentNearestTo(M_home_pos, 5, &dist_opp_to_home );
    const PlayerPtrCont::const_iterator end = wm.teammatesFromSelf().end();
    

    if ( ! opp|| ( wm.existKickableTeammate()&& opp->distFromBall() > 2.5 ) ) {
        return NULL;
    }
    if ( dist_opp_to_home > 0.0 && M_home_pos.x < opp->pos().x ) {
        return NULL;
    }
//}

    for ( PlayerPtrCont::const_iterator it = wm.teammatesFromSelf().begin();it != end;++it ) {
        if ( (*it)->pos().dist( opp->pos() ) < dist_opp_to_home ) {
            return NULL;
        }
    }

    return opp;




    return true;
}

double Bhv_BasicMove_DefensiveHalf::getDashPower( const PlayerAgent * agent) {		//４/１7追記

// ローカル static 変数で回復モードを管理する

	static bool s_recover_mode = false;
	const WorldModel & wm = agent->world();

	if ( wm.self().stamina() < ServerParam::i().staminaMax() * 0.5 ) {  //20180604 0.5から0.3に変更 0619 0.5変更
	s_recover_mode = true;
	}

	else if ( wm.self().stamina()> ServerParam::i().staminaMax() * 0.5 ) {  //20180616 0.5から0.7に変更 0619 0.5変更
	s_recover_mode = false;
	}
	double dash_power = ServerParam::i().maxPower();

// 1 サイクルあたりのスタミナ回復量

	const double my_inc= wm.self().playerType().staminaIncMax() * wm.self().recovery();

if ( wm.ourDefenseLineX() > wm.self().pos().x&& wm.ball().pos().x < wm.ourDefenseLineX() + 20.0 )
 {
// ディフェンスラインよりも後方にいる場合
	dash_power = ServerParam::i().maxPower();
}

 else if ( s_recover_mode ) {
// 回復モードでは 1 サイクル 25 以上の回復量を確保する
	dash_power = std::max( 0.0, my_inc - 25.0 );
}
 else if ( wm.existKickableTeammate()&& wm.ball().distFromSelf() < 20.0 ) {

// 味方が近くでボールを持っていれば,移動速度を早める
	dash_power = std::min( my_inc * 1.1,
	ServerParam::i().maxPower() );
} 
	else if ( wm.self().pos().x > wm.ourOffensePlayerLineX() ) {
// オフサイド位置であれば全力で戻る
dash_power = ServerParam::i().maxPower();
} 

//------------------4/17（相手のオフェンスラインが上がってきたら戻る）-----------------------------
else if(wm.theirOffenseLineX() > wm.self().pos().x && wm.ball().pos().x < wm.theirOffenseLineX() + 10.0){

         dlog.addText( Logger::TEAM,
                      __FILE__": Come_Back" );

	dash_power = std::min( my_inc * 5.0,         //スピードアップ
	ServerParam::i().maxPower() );

	dlog.addText( Logger::TEAM,
                      __FILE__": do_centerBack" );
	
	const BallObject & ball = agent->world().ball();     //ブロック？
	
	Vector2D center( -41.5, 0.0 );
		if ( ball.pos().x < -45.0 ) {
	center.x = std::max( -48.0, ball.pos().x + 1.0 );
	}
	Line2D block_line( wm.ball().pos(),
	( center - wm.ball().pos() ).th() );
		Vector2D block_point;
		block_point.y = std::max( 15.0, ball.pos().absY() - 10.0 );
	if ( ball.pos().y < 0.0 ) block_point.y *= -1.0;
		block_point.x = block_line.getX( block_point.y );

	}

/*else if ( wm.theirDefenseLineX() > wm.self().pos().x&& wm.ball().pos().x < wm.theirDefenseLineX() - 30.0 && wm.ball().pos().x > 0.0 )
{       
	
	dlog.addText( Logger::TEAM,
                      __FILE__": Through_pass_center" );

        dash_power = std::min( my_inc * 3,
	ServerParam::i().maxPower() );

	}*/


	else {
// 通常時は回復量よりも大きいダッシュパワーで移動する
        dash_power = std::min( my_inc * 2.0,ServerParam::i().maxPower() ); //20190704 3.0->1.6
}
	return dash_power;
}

