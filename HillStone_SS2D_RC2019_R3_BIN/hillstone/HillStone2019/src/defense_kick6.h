#ifndef TOKYOTEC_DEFENSE_KICK6_H
#define TOKYOTEC_DEFENSE_KICK6_H

#include <rcsc/player/soccer_action.h>


class DEFENSE_KICK6
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
