#ifndef TOKYOTEC_DEFENSE_KICK4_H
#define TOKYOTEC_DEFENSE_KICK4_H

#include <rcsc/player/soccer_action.h>


class DEFENSE_KICK4
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
