#include "Ocean.h"
#include "MASS_base.h"
#include <sstream>

const bool printOutput = false;

extern "C" Place* instantiate( void *argument ) {
  return new Ocean( argument );
}

extern "C" void destroy( Place *object ) {
  delete object;
}

const int Ocean::neighbor[4][2]={{0,1}, {1,0}, {0,-1}, {-1,0}};
void *Ocean::init( void *argument ) 
{
    if(printOutput)
    {
        ostringstream convert;
        convert << "ocean init";
        MASS_base::log( convert.str( ) );
    }

  //Define cardinals
  //int north[2]  = {0, 1};  cardinals.push_back( north );
  //int east[2] = {1, 0};  cardinals.push_back( east );
  //int south[2]  = {0, -1}; cardinals.push_back( south );
  //int west[2] = {-1, 0}; cardinals.push_back( west ); 
  resetForNextTurn();		//Initialize by clearing the migration status 
  return NULL;
}

void *Ocean::getColorValue( void *argument ) {
  int value = -20; // match to blue
  if(this->fish.agentId != -1)
  {
      value = this->fish.isFish?0:20;//0 green, 20 red
  }

  int *ret = new int; // deleted by the caller
  *ret = value;
  return (void*)ret;
}

void *Ocean::setMyAgent(void *argument)
{
    //Can only set agentID if agent resides on the Land
	if(agents.size() == 0)
		return NULL;
	
    if(argument == NULL)
        return NULL;

if(printOutput)
{
    MASS_base::log( "inside setmyagent");
}
    Fish &param = *(Fish *)argument;
    this->fish.agentId = param.agentId;
    this->fish.isFish = param.isFish;
    delete &param;

    if(!this->fish.isFish)
    {
        this->isShark = true;
    }

    int myAgentType = fish.isFish? 1: 0;

    //set outMessage
    this->outMessage_size = sizeof( int );
    this->outMessage = new int;
    *(int *)outMessage = myAgentType;

	//Debug logging: Expects id to match the agentId on corresponding coordinate
	ostringstream convert;
	convert << "setMyAgent: Land[" << index[0] << "][" << index[1] << "] agent type is (" << myAgentType << ")" << "agent id is " <<this->fish.agentId;
	if(printOutput)
    {
        MASS_base::log( convert.str( ) );
    }
	return NULL;
}

void *Ocean::removeAgent()
{
    this->fish.agentId = -1;
    return NULL;
}

void *Ocean::checkMyNeighbor(void *argument)
{
    if(printOutput)
    {
        MASS_base::log( "inside check my neighbor" );
    }
    int north[2] = {0, 1};
    int east[2] = {1, 0}; 
    int south[2] = {0, -1};
    int west[2] = {-1, 0};
    int *ptr = (int *)getOutMessage( 1, north );
    this->neighborFish[0] = ( ptr == NULL ) ? -1 : *ptr;
    ptr = (int *)getOutMessage( 1, east );
    this->neighborFish[1] = ( ptr == NULL ) ? -1 : *ptr;
    ptr = (int *)getOutMessage( 1, south );
    this->neighborFish[2] = ( ptr == NULL ) ? -1 : *ptr;
    ptr = (int *)getOutMessage( 1, west );
    this->neighborFish[3] = ( ptr == NULL ) ? -1 : *ptr;

    if(printOutput)
    {
        ostringstream convert;
        convert << "checkMyNeighbor:  Land[" << index[0] << "][" << index[1] 
            << "]'s north = " << this->neighborFish[0] << ", east = " << this->neighborFish[1]
            << ", south = " << this->neighborFish[2] << ", west = " << this->neighborFish[3];
        MASS_base::log( convert.str( ) );
    }
    return NULL;
}

void *Ocean::makeReservation(void *argument)
{
    if(agents.size()==0)
    {
        return NULL;
    }
    if(printOutput)
    {
        MASS_base::log("inside makeReservation");
    }

    if(this->fish.isFish)
    {
        this->calculateFishMove();
    }
    else
    {
        this->calculateSharkMove();
    }

	//If any neighbor selected, reserve
	if(!(nextMove[0] == 0 && nextMove[1] == 0))
	{
		int *arg = new int;
		*arg = this->fish.agentId;
        int position = 0;
        if(printOutput)
        {
            ostringstream convert;
            convert << "makeReservation: Land[" << this->index[0] << "][" << this->index[1] << "] position (" << position << ")" << "againt id " <<*arg;
            MASS_base::log( convert.str( ) );
        }
        // doesn't work with cross rank!
		putInMessage( 1, nextMove, position, arg );	
        delete arg;
	}

    return NULL;
}

void *Ocean::checkReservation(void *argument)
{
    ostringstream debug;
	vector<int> reservations;
	int* cur;
    inMessage_size = sizeof(int);
	debug<<"inmessage size" << inMessages.size();
	for(int i = 0; i < (int)inMessages.size(); i++)
	{
		cur = (int *)(inMessages[i]);
		
		//Add whatever is trying to reserve on me
		if(cur != NULL)
		{
			if(*cur != 0)
				reservations.push_back(*cur);
			debug << *cur << ", ";
		}
		else
			debug << "null, ";	
	}
	
	outMessage_size = sizeof( int );
	outMessage = new int( );
	//If empty, outMessage is 0
	if(reservations.empty())
	{
		*(int *)outMessage = 0;
	}
	//Else, outMessage is a randomly selected reservation
	else
	{
		int r = rand() % reservations.size();
		*(int *)outMessage = reservations[r];
	}
	
    if(printOutput)
    {
        //Debug logging: Expects to have read a reservation of whichever agentID wishing to migrate to this Land or 0 if none
        ostringstream convert;
        convert << "checkReservation: Land[" << this->index[0] << "][" << this->index[1] << "] is reserved (" << *(int *)outMessage << ")";
        MASS_base::log( convert.str( ) );
        MASS_base::log( debug.str( ) );
        
    }

	return NULL;
}

void *Ocean::calculateNextMove(void *argument)
{
    int *retVal = NULL;
    if(nextMove[0] == 0 && nextMove[1] == 0)
        retVal = NULL;
    else
    {
        // check whether I got the resevation

        int *ptr = (int *)getOutMessage( 1, nextMove );
        if(!ptr)
        {
            return NULL;
        }
        int agentid = *(int*)argument;
        delete (int*)argument;

        if(printOutput)
        {
        ostringstream convert;
        convert << "calculateNextMove: Land[" << this->index[0] << "][" << this->index[1] << "] move is [" << nextMove[0] << "]["<< nextMove[1] << "] outmessage"<< "agent id" <<agentid;
        MASS_base::log( convert.str( ) );
        }

        //don't check out message, since putInMessage is not working
        if(*ptr == agentid)// this mean it is reserved
        {
            retVal = new int[2];// deleted by caller
            retVal[0] = nextMove[0];
            retVal[1] = nextMove[1];
        }
    }
    return retVal;
}

void Ocean::calculateFishMove()
{
    vector<int> moveset;
    for(int i=0; i<4; i++)
    {
        if(neighborFish[i] == -1)
        {
            if(index[0]==0 && i ==3)
            {
                continue; // skip first row
            }
            
            if(index[1]==0 && i ==2)
            {
                continue;
            }

            if(index[0] == (this->size[0] - 1) && i == 1)
            {
                continue;
            }

            if(index[1]==(this->size[0] -1 )&& i==0)
            {
                continue;
            }

            moveset.push_back(i);
        }
    }
    if(!moveset.empty())
    {
        int r=rand()%moveset.size();
        nextMove[0]=neighbor[moveset[r]][0];
        nextMove[1]=neighbor[moveset[r]][1];
    }

    ostringstream convert;
	if(!(nextMove[0] == 0 && nextMove[1] == 0))
		convert << "makeFishReservation: Ocean[" << index[0] << "][" << index[1] << "] reserving on [" << (index[0] + nextMove[0]) << "][" << (index[1] + nextMove[1]) << "]";
	else
		convert << "makeFishReservation: Ocean[" << index[0] << "][" << index[1] << "] could not make reservation";
    if(printOutput)
    {
	    MASS_base::log( convert.str( ) );
    }
}

void Ocean::calculateSharkMove()
{
    vector<int> towardFishSet;
    vector<int> emptyset;
    for(int i=0; i<4; i++)
    {
        if(index[0]==0 && i ==3)
        {
            continue; // skip first row
        }
        
        if(index[1]==0 && i ==2)
        {
            continue;
        }

        if(index[0] == (this->size[0] - 1) && i == 1)
        {
            continue;
        }

        if(index[1]==(this->size[0] -1 )&& i==0)
        {
            continue;
        }

        if(neighborFish[i] == -1)
            emptyset.push_back(i);
        else if(neighborFish[i] == 1)
            towardFishSet.push_back(i);
    }
    if(!towardFishSet.empty())
    {
        int r = rand()%towardFishSet.size();
        nextMove[0] = neighbor[towardFishSet[r]][0];
		nextMove[1] = neighbor[towardFishSet[r]][1];
    }
    else if(!emptyset.empty())
    {
        int r = rand()%emptyset.size();
        nextMove[0] = neighbor[emptyset[r]][0];
		nextMove[1] = neighbor[emptyset[r]][1];
    }

    ostringstream convert;
	if(!(nextMove[0] == 0 && nextMove[1] == 0))
		convert << "makeSharkReservation: Ocean[" << index[0] << "][" << index[1] << "] reserving on [" << (index[0] + nextMove[0]) << "][" << (index[1] + nextMove[1]) << "]";
	else
		convert << "makeSharkReservation: Ocean[" << index[0] << "][" << index[1] << "] could not make reservation";
	
    if(printOutput)
    {
        MASS_base::log( convert.str( ) );
    }
}

void *Ocean::resetForNextTurn()
{
    for(int i=0; i<4; i++)
    {
        neighborFish[i]=-1;
    }
    nextMove[0]=0; nextMove[1]=0;
    outMessage=NULL;
    outMessage_size=0;
    inMessage_size = sizeof(int);
    inMessages.clear();
    this->isShark = false;
    this->fish.agentId = -1;
    return NULL;
}

void *Ocean::isThereShark()
{
    int *returnValue = new int; // delete by caller
    *returnValue= this->isShark?1:0;
    return returnValue;
}