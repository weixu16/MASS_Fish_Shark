#include "MASS.h"
#include "Timer.h"
#include "Ocean.h"
#include "Fishark.h"
#include <stdlib.h>
#include <unistd.h>
#include <vector>

Timer timer;

int main(int argc, char *args[])
{
	bool printOutput = false;
	bool printOutGraph = true;
    if(argc!=10)
    {
        cerr<<"usage: ./main sername password machinefile port nProc nThreads numChronon sizeX sizeY ratio" << endl;
        return -1;
    }
    char *arguments[4];
    arguments[0] = args[1]; // username
	arguments[1] = args[2]; // password
	arguments[2] = args[3]; // machinefile
	arguments[3] = args[4]; // port
	int nProc = atoi( args[5] ); // number of processes
	int nThr = atoi( args[6] );  // number of threads

    const int numChronon = atoi( args[7] );	//Run this simulation for numChronon
	const int sizeX = atoi( args[8] );
	const int sizeY = atoi( args[9] );
    //const int ratio = atoi( args[10] );  //ratio of shark, between 0~100
    const double rmSpawn = 0.4;
    const int myPopulation = sizeX * sizeY * rmSpawn * rmSpawn;
    static const int totalSize = sizeX * sizeY;	

    MASS::init(arguments, nProc, nThr);

    char *msg = (char *)("ocean\0");

	if(printOutput)
	{
		std::cerr << "create Ocean places " << std::endl;
	}
    Places *ocean = new Places(1, "Ocean", 1, msg, 6, 2, sizeX, sizeY);

	if(printOutput)
	{
		std::cerr << "create Fishark Agents " << std::endl;
	}
    Agents *fishark = new Agents(2, "Fishark", msg, 6, ocean, myPopulation);
    
	if(printOutput)
	{
		std::cerr << "init ocean " << std::endl;
	}
	ocean->callAll(Ocean::init_);

	if(printOutput)
	{
		std::cerr << "init fishark " << std::endl;
	}
    fishark->callAll(Fishark::ramdonDistrubte_);
  	fishark->manageAll();
	fishark->callAll(Fishark::setAgentInPlace_);
	ocean->exchangeBoundary();
	fishark->callAll(Fishark::amIDie_);
	fishark->manageAll();

    timer.start();
    for(int t=0; t<numChronon; t++){
		int* iter = new int;
		*iter = t;
		if(printOutput)
		{
			std::cerr << "run turn " << t << std::endl;
		}
		// reset the information on each place for the next location ordered move.
		ocean->callAll(Ocean::resetForNextTurn_);
		fishark->callAll(Fishark::setAgentInPlace_);
		if(printOutGraph)
		{
			void* colors = ocean->callAll(Ocean::getColorValue_, NULL, 0, sizeof(int));
			int* intcolors = (int*)colors;
			for(int i=0; i< totalSize ; i++)
			{
				std::cout<<intcolors[i]<<" ";
			}
			std::cout<<endl;
			delete[] intcolors;
		}

		if(printOutput)
		{
			std::cerr << "before exchange boundary, neighbor" << std::endl;
		}
		// ocean->exchangeBoundary();
		if(printOutput)
		{
			std::cerr << "before check my neighbor" << std::endl;
		}
		ocean->callAll(Ocean::checkMyNeighbor_);
		ocean->callAll(Ocean::makeReservation_);
		//ocean->exchangeBoundary();

		ocean->callAll(Ocean::checkReservation_);
        ocean->exchangeBoundary();

 		fishark->callAll(Fishark::move_);

        fishark->manageAll();

		fishark->callAll(Fishark::setAgentInPlace_);
		ocean->exchangeBoundary();

		fishark->callAll(Fishark::amIDie_);
		fishark->manageAll();
		
		fishark->callAll(Fishark::createChild_, (void*) iter, sizeof(int));

		fishark->manageAll();
		
		delete iter;
	}
	long elaspedTime_END =  timer.lap();
	if(printOutput)
	{
	printf( "\nEnd of simulation. Elasped time using MASS framework with %i processes and %i thread and %i turns :: %ld \n",nProc,nThr,numChronon, elaspedTime_END);
	}
    MASS::finish( );
}
