#ifndef OCEAN_H
#define OCEAN_H

#include <string.h>
#include "Place.h"
using namespace std;

class Fish {
    public:
    Fish() : agentId(-1), isFish(false){ };
    Fish( int id, bool isf ) :
    agentId( id ), isFish( isf ){ };
    int agentId;
    bool isFish; // ture is fish, false is shark
};

class Ocean : public Place {
public:
    static const int init_ = 0; 
    static const int getColorValue_ = 1;
    static const int checkMyNeighbor_ = 2;
    static const int calculateNextMove_ = 3;
    static const int resetForNextTurn_ = 4;
    static const int setMyAgent_ = 5;
    static const int removeAgent_ = 6;
    static const int isThereShark_ = 7;
    static const int makeReservation_ = 8;
    static const int checkReservation_ = 9;

    Ocean( void *argument ) : Place( argument ) {
    };

    virtual void *callMethod(int functionId, void *argument)
    {
        switch(functionId)
        {
            case init_: return init(argument);
            case getColorValue_: return getColorValue(argument);
            case checkMyNeighbor_: return checkMyNeighbor(argument);
            case calculateNextMove_: return calculateNextMove(argument);
            case resetForNextTurn_: return resetForNextTurn();
            case setMyAgent_: return setMyAgent(argument);
            case removeAgent_: return removeAgent();
            case isThereShark_: return isThereShark();
            case makeReservation_: return makeReservation(argument);
            case checkReservation_: return checkReservation(argument);
        }
        return NULL;
    };
private:
    void *init(void *argument);
    void *getColorValue(void *argument);
    void *checkMyNeighbor(void *argument);
    void *calculateNextMove(void *argument);
    void calculateFishMove();
    void calculateSharkMove();
    void *resetForNextTurn();
    void *setMyAgent(void *argument);
    void *removeAgent();
    void *isThereShark();
    void *makeReservation(void *argument);
    void *checkReservation(void *argument);

    vector<int*> cardinals;
    static const int neighbor[4][2];

    Fish fish;
    int neighborFish[4]; //store the fish type of the neighbor, 1 is fish, 0 is shark, -1 no fish
    int nextMove[2];
    bool isShark;
};
#endif