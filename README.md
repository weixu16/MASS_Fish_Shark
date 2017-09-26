# MASS_Wa-Tor
Multi-Agent Spatial Simulation of Wa-Tor

## MASS (Multi-Agent Spatial Simulation)

The MASS library, the manual, and sample programs are found:
http://depts.washington.edu/dslab/MASS/index.html

## Wa-Tor
http://en.wikipedia.org/wiki/Wa-Tor
Observes the population increase/decrease of predator and prey agents in the ocean.


## Implementation 
![Alt text](https://github.com/weixu16/MASS_Wa-Tor/blob/master/picture/project%20overview.png)

The Ocean class inherits from the Place, it represents a cell in the ocean, it can hold one fish or one shark. It implements below functions:

1. checks the neighbor cell to decide where to move fish/shark; 
2. calculate where to move, fish moves to an empty space, shark moves toward fish;
3. put AgentID to the InMessage of the neighbor to make reservation;
4. check reservation, by read InMessage and then put it to the OutMessage. 

The Fishark class inherits from the Agent class, it represents a fish or a shark, based on its isFish property. 
1. setAgentInplace, tell the ocean, that I am live there, by set the agentid and isFish property of the Ocean
2. move, ask ocean, where I can move and move it to there
3. do I die, check whether there is shark moved to my space, if so, I am dead.
4. create child, born fish or shark child. 

The main function, it reads in parameters, and setup Oceans, fisharks, and then run the loop to simulate it. 

## Performance Evaluation
