#ifndef TOKYOTEC_DEFENSE_KICK5_H
#define TOKYOTEC_DEFENSE_KICK5_H

#include <rcsc/player/soccer_action.h>


class DEFENSE_KICK5
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
