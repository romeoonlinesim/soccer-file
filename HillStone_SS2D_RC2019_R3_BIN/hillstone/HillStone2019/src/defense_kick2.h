#ifndef TOKYOTEC_DEFENSE_KICK2_H
#define TOKYOTEC_DEFENSE_KICK2_H

#include <rcsc/player/soccer_action.h>


class DEFENSE_KICK2
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
