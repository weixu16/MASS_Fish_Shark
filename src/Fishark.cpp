#include "Fishark.h"
#include "Ocean.h"
#include "MASS_base.h"
#include <sstream>
#include <math.h>

const bool printOutput=false;
extern "C" Agent* instantiate( void *argument ) 
{
  return new Fishark( argument );
}

extern "C" void destroy( Agent *object ) 
{
  delete object;
}

Fishark::Fishark(void* argument) : Agent( argument )
{
    this->agentInit(argument);
}

void *Fishark::agentInit(void *argument)
{
    int type = -1;
    if(argument)
    {
        type = *(int*)argument;
    }

    ostringstream convert;
    if(type == 1)
    {
        if(printOutput == true)
        {
            convert << "create child fish[" << "]" <<endl;
            MASS_base::log( convert.str( ) );
        }

        this->isFish=true;
    }
    else if(type == 0)
    {
        if(printOutput == true)
        {
            convert << "create child shark[" << "]"<<endl;
            MASS_base::log( convert.str( ) );
        }
        this->isFish=false;
        this->starveIndex=100;
    }
    else
    {
        int r = rand()%100;
        int ratio = 80;
        if(r<ratio)
        {
            if(printOutput == true)
            {
                convert << "create fish[" << "]" <<endl;
                MASS_base::log( convert.str( ) );
            }

            this->isFish=true;

            /*
            int* idpointer = new int;
            *idpointer = this->agentId;
            this->place->callMethod(Ocean::setMyAgentId_, (void*) idpointer);
            int* typepointer = new int;
            *typepointer = 1;
            this->place->callMethod(Ocean::setMyAgentType_, (void*) typepointer);
            */
        }
        else
        {
            if(printOutput == true)
            {
                convert << "create shark[" << "]"<<endl;
                MASS_base::log( convert.str( ) );
            }
            this->isFish=false;
            this->starveIndex=100;
        }
    }



    if(printOutput == true)
    {
      convert << "agentInit for agentid [" << agentId << "] called"<<endl;
      MASS_base::log( convert.str( ) );
    }

    return NULL;
}

void *Fishark::setAgentInPlace()
{
    if(printOutput == true)
    {
        MASS_base::log( "in side setAgentInPlace" );
      ostringstream convert;
      convert << "setAgentInPlace [" << agentId << "] called"<<endl;
      MASS_base::log( convert.str( ) );
    }

    Fish *param = new Fish( this->agentId, this->isFish ); // deleted in setmyagent
    this->place->callMethod(Ocean::setMyAgent_, (void*) param);
    return NULL;
}

/**
 * creates a child agent from a parent agent.How to setup position for them????
 */
void *Fishark::createChild(void *argument)
{
    if(isFish)
    {
        this->createChild_fish(argument);
    }
    else
    {
        this->createChild_shark(argument);
    }
    return NULL;
}

void Fishark::killMe()
{
    this->place->callMethod(Ocean::removeAgent_, NULL);
    kill();
    ostringstream convert;
    if(printOutput == true){
        convert << "killMe[" << agentId << "] called, is fish: "<<this->isFish<<endl;
        MASS_base::log( convert.str( ) );
    }
}

void Fishark::createChild_fish(void *argument )
{
    int intiturn = *(int*)argument;
    if( intiturn%10 == 0&& intiturn!=0)
    {
        vector<void *> *arguments = new vector<void *>;
        int* value = new int;
        *value = 1;
        arguments->push_back((void*)value);
        spawn(1, *arguments,15);
        delete value;
        delete arguments;

        ostringstream convert;
        if(printOutput == true){
            convert << "create fish child[" << this->agentId << "] called to spawn " << 1 << " turn " << intiturn;
            MASS_base::log( convert.str( ) );
        }
    }
}

void Fishark::createChild_shark(void *argument )
{
    int intiturn = *(int*)argument;
    if( intiturn%25 == 0&&intiturn!=0)
    {
        vector<void *> *arguments = new vector<void *>;
        int* value = new int;
        *value = 0;
        arguments->push_back((void*)value);
        spawn(1, *arguments,15);
        delete value;
        delete arguments;

        ostringstream convert;
        if(printOutput == true){
            convert << "create shark child[" << this->agentId << "] called to spawn " << 1 << " turn " << intiturn;
            MASS_base::log( convert.str( ) );
        }
    }
}

void *Fishark::printCurrentWithID( void *argument ) 
{
    int* turnPtr = (int*) argument;
    if(printOutput == true)
    {
        ostringstream convert;
        convert<< "Turn " <<*turnPtr << " agent " << agentId << " current Location = [" << this->place->index[0] << "][" << this->place->index[1] << "]" << endl;
        MASS_base::log(convert.str());
    }
    return NULL;
}

void *Fishark::move()
{
    ostringstream convert;
    if(printOutput)
    {
        MASS_base::log("inside moveFish");
    }

    vector<int> dest;
    int x = 0, y = 0;

    int agentId = this->agentId;
    int *argument = new int;// deleted by calculateNextMove_
    *argument= agentId;

    int *nextMove = (int *)(place->callMethod(Ocean::calculateNextMove_, (void*)argument));
    if(nextMove != NULL)
    {
        if(!(nextMove[0]==0 && nextMove[1]==0))
        {
            x=place->index[0] + nextMove[0];
            y = place->index[1] + nextMove[1];
            dest.push_back( x );
            dest.push_back( y );

            if(printOutput == true)
            {
                MASS_base::log("remove me before move");
            }
            // remove 
            this->place->callMethod(Ocean::removeAgent_, NULL);
            migrate(dest);

            convert << "Move: Agent-" << agentId << " at [" << place->index[0];
            convert << ", " << place->index[1] << "] to move to [" << x << ", ";
            convert << y << "] a displacement of [" << nextMove[0] << ", " << nextMove[1] << "]";
        }
        else
        {
            convert << "can't move agent " << agentId; 
        }
        delete[] nextMove;
    }
    else 
    {
		convert << "Move: Agent-" << agentId << " at [" << place->index[0] << ", " << place->index[1] << "] could not move!";
	}

	if(printOutput) { 
		MASS_base::log( convert.str( ) ); 
	}
	
	return NULL;
}

void *Fishark::amIDie()
{
    if(printOutput == true)
    {
        MASS_base::log("inside amIDie");
    }
    ostringstream convert;
    convert<< "amIDie : " << " agent " << agentId << " isFish "<< this->isFish << " current Location = [" << this->place->index[0] << "][" << this->place->index[1] << "]" << endl;
    

    if(this->isFish)
    {
        //check whether there is shark there
        void* value =  place->callMethod(Ocean::isThereShark_, NULL);
        int isThereShark = *(int*)value;
        delete (int*)value;
        if(isThereShark ==1)
        {
            convert << "There is shark kill me"<<endl;
            this->killMe();
        }
    }
    else
    {
        convert <<"shark starve index is " << this->starveIndex <<endl;
        this->starveIndex--;
        if(this->starveIndex < 0)
        {
            convert <<"shark starve "<< "Agentid" << this->agentId << " at [" << place->index[0] << ", " << place->index[1] << "]";
            this->killMe();
        }
    }

    if(printOutput == true)
    {
        MASS_base::log(convert.str());
    }
    return NULL;
}

void *Fishark::ramdonDistrubte()
{
    int x = rand()%(this->place->size[0]);
    int y = rand()%(this->place->size[1]);

    vector<int> dest;
    dest.push_back( x );
    dest.push_back( y );

    migrate(dest);

    return NULL;
}