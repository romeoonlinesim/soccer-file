#ifndef TOKYOTEC_OFFENSE_KICK6_H
#define TOKYOTEC_OFFENSE_KICK6_H

#include <rcsc/player/soccer_action.h>


class OFFENSE_KICK6
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
