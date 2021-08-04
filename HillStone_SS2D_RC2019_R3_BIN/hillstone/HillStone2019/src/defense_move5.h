#ifndef DEFENSE_MOVE5_H/*変更*/
#define DEFENSE_MOVE5_H/*変更*/

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

class DEFENSE_MOVE5/**/
    : public rcsc::SoccerBehavior {
public:
    DEFENSE_MOVE5()/*変更*/
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:
    double getDashPower( const rcsc::PlayerAgent * agent );
};

#endif
