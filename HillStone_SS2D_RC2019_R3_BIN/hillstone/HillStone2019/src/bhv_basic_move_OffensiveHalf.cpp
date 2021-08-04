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

#include "bhv_basic_move_OffensiveHalf.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>
#include <rcsc/action/body_dribble.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h> 

#include "neck_offensive_intercept_neck.h"
#include <rcsc/action/body_pass.h>

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicMove_OffensiveHalf::execute( PlayerAgent * agent )
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
		     || (*t)->posCount() > wm.ball().posCount() + 3 )
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
		    
		    M_second_teammate = *t;
		    dlog.addText( Logger::INTERCEPT,
				  "---> set bhv_basic_move %d (%.1f %.1f)",
				  (*t)->unum(),
				  (*t)->pos().x, (*t)->pos().y );
		    break;
		  }
	      }
//------------------------------------------------------------------------

	    if ( M_fastest_teammate )
	      {
		dlog.addText( Logger::INTERCEPT,
			      "<-----Intercept Teammate  bhv_basic_move = %d."
			      " teammate %d (%.1f %.1f)",
			      M_teammate_reach_cycle,
			      M_fastest_teammate->unum(),
			      M_fastest_teammate->pos().x,
			      M_fastest_teammate->pos().y );
		
		if (M_fastest_teammate || (M_second_teammate->unum() == 10 || M_second_teammate->unum() == 8 || M_second_teammate->unum() == 9 && M_second_teammate->pos().x,M_second_teammate->pos().y))
		  {
		    dlog.addText( Logger::TEAM,
				  __FILE__": 1_2pass_1" );
		    
		    Body_Pass();
		    ServerParam::i().maxPower();
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
		
		if (M_second_teammate || (M_fastest_teammate->unum() == 10  || M_second_teammate->unum() == 8 || M_second_teammate->unum() == 9 && M_fastest_teammate->pos().x,M_fastest_teammate->pos().y))
		  {
		    dlog.addText( Logger::TEAM,
				  __FILE__": 1_2pass_2" );
		    
		    Body_Pass();
		    Body_Pass();
		    ServerParam::i().maxPower();
		  }
	      }
	  }
//-----------------------------------------------------------------------------
	const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
	const double dash_power = Bhv_BasicMove_OffensiveHalf::getDashPower( agent );//Strategy::get_normal_dash_power( wm );
	
	double dist_thr = wm.ball().distFromSelf() * 0.1;
	if ( dist_thr < 1.0 ) dist_thr = 1.0;
	
	dlog.addText( Logger::TEAM,
		      __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
		      target_point.x, target_point.y,
		      dist_thr );
	
	agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
	agent->debugClient().setTarget( target_point );
	agent->debugClient().addCircle( target_point, dist_thr );
	
	Vector2D drib_target( 50.0, 0.0 );
	
	AngleDeg drib_angle = ( drib_target - wm.self().pos() ).th();
	
	const Sector2D sector( wm.self().pos(),
			       0.5, 15.0,
			       drib_angle - 30.0, drib_angle + 30.0);
	
	if (  wm.existOpponentIn( sector, 1000, true ) ) {
	  
	  dlog.addText( Logger::TEAM,
			__FILE__": holder_method" );
	  
	  Body_Dribble( drib_target, 1.0, 
		ServerParam::i().maxPower(), 5);
	}
	
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
	
	const PlayerObject * getMarkTarget( const WorldModel & wm,const Vector2D & home_pos ) ;
	double dist_opp_to_home = 500.0;
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

//  
//}

double Bhv_BasicMove_OffensiveHalf::getDashPower( const PlayerAgent * agent) {		//４/１2追記
  
  // ローカル static 変数で回復モードを管理する
  
  static bool s_recover_mode = false;
  const WorldModel & wm = agent->world();
  
  if ( wm.self().stamina() < ServerParam::i().staminaMax() * 0.5 ) {  //20180616 0.5から0.3に変更　0619 0.5戻し
    s_recover_mode = true;
  }
  
  else if ( wm.self().stamina()> ServerParam::i().staminaMax() * 0.5 ) {  //20180619 0.7から0.5に変更
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
    dash_power = std::min( my_inc * 2,
			   ServerParam::i().maxPower() );
  } 
  else if ( wm.self().pos().x > wm.ourOffensePlayerLineX() ) {
    // オフサイド位置であれば全力で戻る
    dash_power = ServerParam::i().maxPower();
  } 
  
  //4/17追記/*-----飛び出し--------*/
  
  else if ( wm.theirDefenseLineX() > wm.self().pos().x&& wm.ball().pos().x < wm.theirDefenseLineX() - 30.0 && wm.ball().pos().x > 0.0 )
    {       
      
      dlog.addText( Logger::TEAM,
		    __FILE__": Through_pass_center" );
      
      dash_power = std::min( my_inc * 5,
			     ServerParam::i().maxPower() );//20190625 5->3
      
    }
  else if(wm.theirOffenseLineX() > wm.self().pos().x && wm.ball().pos().x < wm.theirOffenseLineX() + 20.0){
    
    dlog.addText( Logger::TEAM,
		  __FILE__": Come_Back" );
    
    dash_power = std::min( my_inc * 3.0,         //スピードアップ 20190625 3.0->2.0
			   ServerParam::i().maxPower() );
    
    /*dlog.addText( Logger::TEAM,
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
      block_point.x = block_line.getX( block_point.y );*/
    
  }
  
  
  
  else {
    // 通常時は回復量よりも大きいダッシュパワーで移動する
    dash_power = std::min( my_inc * 2.0,ServerParam::i().maxPower() );// 5/2 3 20190625 3->1.6
  }
  
  
  return dash_power;
}


//ホルダー側

void getholder(const PlayerAgent * agent,
               const PlayerObject * receiver,
               const WorldModel & world){
  
  const WorldModel & wm = agent->world();
  
  dlog.addText( Logger::TEAM,
		__FILE__": holder_method" );
  
  //makeVector2D pass_target( 50.0, 0.0 );
  AngleDeg pass_angle =  (receiver->pos() - world.self().pos() ).th();
  const Sector2D sector( wm.self().pos(),0.5, 15.0,pass_angle - 30.0, pass_angle + 30.0);
}

void DoDri(const PlayerAgent * agent){
  const WorldModel & wm = agent->world();
  Vector2D drib_target( 50.0, 0.0 );
  AngleDeg drib_angle = ( drib_target - wm.self().pos() ).th();
  const Sector2D sector( wm.self().pos(),
			 0.5, 15.0,
			 drib_angle - 30.0, drib_angle + 30.0);
  
  
  if (  wm.existOpponentIn( sector, 10, true ) ) {
    Body_Dribble( drib_target, 1.0, 
		  ServerParam::i().maxPower(), 5);
  }
}

//レシーバ
/*void getreciver(const WorldModel & world,
  const Vector2D & receiver_pos,
  const Vector2D & target_point,
  const double & target_dist,
  const AngleDeg & target_angle,
  const double & first_speed){
  
  }*/



