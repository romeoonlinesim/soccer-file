#ifndef OFFENSE_MOVE3_H/*変更*/
#define OFFENSE_MOVE3_H/*変更*/

#include <rcsc/geom/vector_2d.h>
#include <rcsc/player/soccer_action.h>

class OFFENSE_MOVE3/**/
    : public rcsc::SoccerBehavior {
public:
    OFFENSE_MOVE3()/*変更*/
      { }

    bool execute( rcsc::PlayerAgent * agent );

private:
    double getDashPower( const rcsc::PlayerAgent * agent );
};

#endif
