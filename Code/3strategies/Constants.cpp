/* 
 * File:   Constants.cpp
 * Author: sduca
 * 
 * Created on February 4, 2015, 2:06 PM
 */


#include<cstdio>
#include<cstdlib>
#include<fstream>
#include "Constants.h"

using namespace std;
/*

Constants::Constants(){
	N=100; //Number of agents playing the game
	T=100;//Time at which I want to stop my simulation
        interval=0.1; //time interval to print my data
	S=4; //Size of the groups
	Q=0.4; //Return rate
	lambda=0.1; //Parameter for the Exponential distribution, the mean
	mu=1; //Mean of the Gaussian distribution for the talent
	sigmag=0.22; //Variance sigma of the Gaussian distribution for the tale
}
Constants::~Constants(){}

*/

//This reads from a file:

Constants::Constants(){ //Note that name must be the entire path; i.e. "./config.conf"
	char line[256];
	int linenum=0;
	int count=0, M=8; //M is the amount of parameters I have to give, count will range from 0 to M-1
	double vector[M]; //will store the M parameters
	FILE *pfile;

	pfile = fopen ("./config.conf" , "r");
	//pfile= fopen("/project/theorie/s/Stefano.Duca/Analysis/Prog/config.conf", "r"); //Here I have to put the folder where the config file will be!

	while(fgets(line, 256, pfile) != NULL)
	{
		linenum++;
		if(line[0] == '#') {continue;} //I'm going to the next line without reading and incrementing count
		sscanf(line, "%*s %lf", &vector[count]);
		count ++;
	}
	
	//Here I cast when I actually have integers
	N=int(vector[0]); //Number of agents
	T=int(vector[1]);//Number of time steps
	L=int(vector[2]); //Type of lattice: 0 for Von Neumann square lattice; 1 for Moore square lattice; 2 for ring
	delta=vector[3]; //value of the contract
	beta=vector[4]; //beta value for the logit
	r=vector[5]; //cooperation reward
    a=vector[6]; //sucker's payoff
    mu=vector[7]; //mutation probability
}
Constants::~Constants(){}
