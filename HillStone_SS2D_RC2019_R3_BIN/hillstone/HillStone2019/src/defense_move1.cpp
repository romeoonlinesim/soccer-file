
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "defense_move1.h"
#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
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
DEFENSE_MOVE1::execute( PlayerAgent * agent )
{
	dlog.addText( Logger::TEAM,
					__FILE__": DEFENSE_MOVE1" );

 // tackle

if ( Bhv_BasicTackle( 0.7, 68.1 ).execute( agent ) ){
    return true;
}

const WorldModel & wm = agent->world();
/*--------------------------------------------------------*/
// chase ball
const int self_min = wm.interceptTable()->selfReachCycle();
const int mate_min = wm.interceptTable()->teammateReachCycle();
const int opp_min = wm.interceptTable()->opponentReachCycle();

if ( ! wm.existKickableTeammate()
         && ( self_min <= 7 
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

const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
const double dash_power = Strategy::get_normal_dash_power( wm );

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

return true;
}
