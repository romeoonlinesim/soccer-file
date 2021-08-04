#ifndef TOKYOTEC_OFFENSE_KICK3_H
#define TOKYOTEC_OFFENSE_KICK3_H

#include <rcsc/player/soccer_action.h>


class OFFENSE_KICK3
    : public rcsc::SoccerBehavior {
private:

public:

    bool execute( rcsc::PlayerAgent * agent );
};

#endif
