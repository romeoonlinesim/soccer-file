#ifndef TOKYOTEC_DEFENSE_KICK1_H
#define TOKYOTEC_DEFENSE_KICK1_H

#include <rcsc/player/soccer_action.h>


class DEFENSE_KICK1
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
