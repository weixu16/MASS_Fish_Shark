#ifndef FISHARK_H
#define FISHARK_H

#include "Agent.h"
#include "MASS_base.h"
#include <sstream>

using namespace std;
class Fishark : public Agent {
public:
    static const int agentInit_ = 0;
    static const int createChild_ = 1;
    static const int printCurrentPositionWithID_ = 3;
    static const int move_ = 4;
    static const int setAgentInPlace_ = 5;
    static const int amIDie_ = 6;
    static const int ramdonDistrubte_ = 7;

    static const int cardinals[5][2];

    Fishark( void *argument );
    //Fishark( void *argument ) : Agent( argument ) {
    //if(printOut == true)
    //    MASS_base::log( "BORN!!" );
    //};
  
    virtual void *callMethod( int functionId, void *argument )
    {
        switch(functionId)
        {
            case agentInit_: return agentInit(argument);
            case createChild_: return createChild(argument);
            case printCurrentPositionWithID_: return printCurrentWithID(argument);
            case move_: return move();
            case setAgentInPlace_: return setAgentInPlace();
            case amIDie_: return amIDie();
            case ramdonDistrubte_: return ramdonDistrubte();
        }
        return NULL;
    }

//virtual int map( int initPopulation, vector<int> size, vector<int> index, Place* curPlace);

private:
    bool isFish;
    int starveIndex;
    void *printCurrentWithID( void *argument );
    void *agentInit( void *argument );
    void *createChild( void *argument );
    void createChild_fish( void *argument );
    void createChild_shark( void *argument );
    void killMe();
    void *move();
    void *setAgentInPlace();
    void *amIDie();
    void *ramdonDistrubte();
};
#endif