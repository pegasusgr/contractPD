/* 
 * File:   contractpd.cpp
 * Author: rigosa
 *
 * Created on September 9, 2015, 6:05 PM
 */

// Standard C++ library includes 
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<math.h>
#include<iostream>
#include <iomanip>
#include <new>
#include <algorithm>    
#include <vector> 

// Gnu Scientific Library (gsl) includes
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h> //Needed for the beta pdf

// My includes
#include "Fcts.h"
#include "Constants.h"

using namespace std;

int main() {
	
	// *********** Initialising fields and variables *********** //
	// RNG Stuff 	
	gsl_rng *gslpointer; 	// Pointer to the type of rng
	FILE *pfile; 		// File to read from /usr/urandom
	unsigned int seed; 	// Seed of the random number generator

	// My Constants instance for reading/holding all the input varibles 	
	Constants cons;
	
	int i,t ; //agent and time
	int oldstrategy[cons.N]; //Array with the strategies of the previous round. 0 is C, 1 is D and 2 is delta
	int newstrategy[cons.N]; //Array with the strategies of the current round. 0 is C, 1 is D and 2 is delta
	double welfare, perc, perd, perdelta; //The total welfare variable and the percentages of players playing each strategy
	
	// ofstream for outputing and output files
	ofstream filep;		// Parameter output stream 
	ofstream filet;		// Main/time output stream. Here we write the time, the total welfare and the percentage of players playing each strategy for every time step
	ofstream fileag; 	// Output stream time where we store the strategy of each agent at each time step!
	
	const char filenamep[]="parameters.txt";	// Parameter output filename	
	const char filenamet[]="time.txt";		// Main/time output filename
	const char filenameag[]="agents.txt"; 		// Agents output file name
	
	
	// *********** Initialising the GSL Mersenne twister RNG & a C srand RNG *********** //

	// Print for testing
	cout << "Initialising the RNG" << endl;


    // GSL Mersenne Twister 19937 RNG
	pfile = fopen ("/dev/urandom", "r");
	i=fread (&seed, sizeof (seed), 1, pfile);	// I added the rand= ... just to not be bothered anymore by the warnings!
	fclose(pfile);
	gslpointer = gsl_rng_alloc(gsl_rng_mt19937); 	// I'm using the "Mersenne Twister" generator!
	gsl_rng_set(gslpointer,seed); 			// Starting the generator
	

	// *********** Initialising the OUTPUT FILES with HEADERS *********** //

	// Print for testing
	cout << "Initialising the output files" << endl;
	
	// The TIME file
	filet.open(filenamet,ios::out|ios::trunc); //Open the time file
	if(filet.is_open()){
		filet << "#Results for the simulation contractpd with:"<<endl;
		filet << "#N=" << cons.N << " T=" << cons.T  << " L=";
		filet << cons.L << " delta=" << cons.delta;
		filet << " beta=" << cons.beta << " r=" << cons.r;
		filet << " a=" << cons.a << endl;
		filet << "#This is in the form of t, welfare, C percentage, D percentage and delta percentage" << endl;
	}
	
	// The AGENTS file
	fileag.open(filenameag,ios::out|ios::trunc); //Open the time file
	if(fileag.is_open()){
		fileag << "#Results for the simulation contractpd with:"<<endl;
		fileag << "#N=" << cons.N << " T=" << cons.T  << " L=";
		fileag << cons.L << " delta=" << cons.delta;
		fileag << " beta=" << cons.beta << " r=" << cons.r;
		fileag << " a=" << cons.a << endl;
		fileag << "#This is in the form of a line for every time step. 0 is C, 1 is D and 2 is delta" << endl;
	}	
	
	// The PARAMETER file	
	filep.open(filenamep,ios::out|ios::trunc);
	printparamsingleloop(filep,cons);
	filep.close();
	
	//************ Initialize the strategy arrays and the variables. Printing things at time =0 ****
	for(i=0; i < cons.N ; i++){
		oldstrategy[i]=0;
		newstrategy[i]=0;	
	}
	
	welfare = 0.;
	perc = 0.;
	perdelta = 0.;
	perd = 1. ;
	
	// Now I fill up the output files at t=0
	printstuffsingleloop(filet, fileag, 0, welfare, perc, perd, perdelta, newstrategy ,cons); 	// Print time, welfare, percentages and the state of all agents at time t=0
	//********************************************************************************************************************//
	
	/**************** HERE STARTS THE BIG TIME LOOP **********************/
	
	for(t=0; t<cons.T ; t++){
		
		//Here players update their strategies
		updatestrategy(oldstrategy, newstrategy, cons, gslpointer);
		//Here I compute the total welfare and the various percentages.
		computetotalwelfare(newstrategy, welfare, perc, perd, perdelta, cons);
		//Here I print the state of the system at time t
		printstuffsingleloop(filet, fileag, t, welfare, perc, perd, perdelta, newstrategy ,cons);
		nextround(oldstrategy,newstrategy,cons);

	}
	
	/********************* END OF THE BIG TIME LOOP **************************/
	
	//***** Close all the ofstreams
	filet.close();
	fileag.close();
	
	return 0;
}
