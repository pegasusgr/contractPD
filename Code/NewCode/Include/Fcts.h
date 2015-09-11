#ifndef FCTS_H
#define	FCTS_H

#endif	/* FCTS_H */

#pragma once

// Standard C++ library includes 
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<cmath>
#include<iomanip>
#include <algorithm>    
#include <vector>       
#include <new> 

// Gnu Scientific Library (gsl) includes
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h> //Needed for the beta pdf

// My includes
#include "Constants.h"
#include "neighbors.h"



using namespace std;

/************************* Function that prints all the parameters in a file **************************************/
void printparamsingleloop(ofstream& filec, Constants cons){
	filec<<"#Number of agents:"<<endl;
	filec<<"N= "<<cons.N<<endl<<endl;
	filec<<"#Number of time steps:"<<endl;
	filec<<"T= "<<cons.T<<endl<<endl;
	filec<<"#Type of lattice: 0 for Von Neumann square lattice; 1 for Moore square lattice; 2 for ring:"<<endl;
	filec<<"L= "<<cons.L<<endl<<endl;
	filec<<"#value of the contract:"<<endl;
	filec<<"delta= "<<cons.delta<<endl<<endl;
	filec<<"#beta value for the logit:"<<endl;
	filec<<"beta= "<<cons.beta<<endl<<endl;
	filec<<"#cooperation reward:"<<endl;
	filec<<"r= "<<cons.r<<endl<<endl;
    filec<<"#sucker's payoff:"<<endl;
	filec<<"a= "<<cons.a<<endl<<endl;
	return;
}
/*****************************************************************************************/

/************************* Function that prints the various output at every time loop. ****************************

In particular, it prints time, welfare and percentages on one file and the state of all agents at time t on another file */

void printstuffsingleloop(ofstream& filet, ofstream& fileag, int t, double welfare, double perc, double perd, double perdelta, int *strategy ,Constants cons){
	
	int i; //dummy variable
	
    filet<<t<<" "<<welfare<<" "<<perc<<" "<<perd<<" "<<perdelta<<endl; //Here I print time, welfare and percentages.
	
    for(i=0;i<cons.N;i++){ //Here I print the strategy for each agent at each time step and.
        fileag<<strategy[i]<<" ";
    }
    
    fileag<<endl;
 
    return;
}
/**************************************************************************/

/*********************************************/
int binaryprobsearch(double *Gamma, int M, double x) { //Binary search. Gamma is the array of cumulative prob. M is the lenght of the array and x is the random number between zero and one
    int a, b, l, result;
    bool check;
    a = 0;
    b = M - 1;
    
    do {
        l = (a + b) / 2;
        if (x <= Gamma[l]) {
            if ((x >= Gamma[l - 1])&&(l > 0)) {
                result = l;
                check = true;
            } else {
                if (l > 0) {
                    b = l;
                    check = false;
                } else {
                    result = 0;
                    check = true;
                }
            }
        } else {
            if (x <= Gamma[l + 1]) {
                result = l + 1;
                check = true;
            } else {
                a = l;
                check = false;
            }
        }
    } while (check == false);
    return result;
}
/******************************************************************/

/****************** Function that updates other people strategy *************************/
void updatestrategy(int *oldstrategy, int *newstrategy, Constants cons, gsl_rng *gslpointer){
	
	int M=3; //The number of strategies (for sake of generalization)
	int i;
	int nc, nd, ndelta, totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	double randomnum; //A random number
	double probarr[M]; //Here I will store the probability for each of the strategies to be played
	double expected[M]; //Here I will store the expected utility for each of the strategies
	double sum; //sum variable
	double cumprob[M]; //The array of cumulative probabilities
	int k; //dummy variable
	
	//Here I update the strategy for each of the agents

	for(i=0; i< cons.N; i++){
		//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
		neinum=neighborstrategies(i, oldstrategy, cons);	
		nd=neinum[0];
		nc=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Now I compute the expected utility for each of the strategies
		expected[0]= (nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //If cooperate: nc*r - nd*a + ndelta*(r+delta)
		expected[1]= (nc + ndelta)/totnn; //If defect: nc*1 + nd*0 + ndelta*1
		expected[2]= (nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //If delta: nc*(r-delta) - nd*a + ndelta*r
		
		/******Now I compute the probabilities for each of the strategies*****/
		sum = 0;
		for(k=0; k < M; k++){
			probarr[k] = exp(cons.beta * expected[k]);
			sum = sum + probarr[k];
		}
		//Here I renormalize but I have to do the first by hand due to cumprob
		probarr[0] = probarr[0]/sum;
		cumprob[0]=probarr[0];
		for(k=1; k < M; k++){
			probarr[k] = probarr[k]/sum;
			cumprob[k]=cumprob[k-1] + probarr[k];
		}
		// Now I sample the random number:
		randomnum = gsl_ran_flat(gslpointer,0,1); //Generate a random number btw 0 and 1. Note that I use utility here just to not use another variable
    	k=binaryprobsearch(&cumprob[0],M,randomnum); //Here I compute which strategy is the agent i is using. Note that I use k here just to not use another variable
		//Hence:
		newstrategy[i] = k;
	}
	
	return ;
}
/*********************************************************************************************************/

/****************Function that computes the welfare and the percentage of each strategy on the lattice *********/
void computetotalwelfare(int *newstrategy, double &welfare, double &perc, double &perd, double &perdelta,Constants cons){
	
	int i;
	double sum; //sum variable
	int nc, nd, ndelta, totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	int countc, countd, countdelta; //Integers used for counting
	
	
	sum = 0;
	countc=0;
	countd=0;
	countdelta=0;
	
	for(i=0; i< cons.N; i++){
		//getnumber of each thing (as a fct of i, newstrategy and the type of lattice);
		neinum=neighborstrategies(i, newstrategy, cons);	
		nd=neinum[0];
		nc=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Check which strategy the player is using and update the utility accordingly
		switch(newstrategy[i]){
			
			case 0:
				sum = sum + (nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //The payoff that you get from cooperating
				countc ++;
				break;
			
			case 1:
				sum = sum + (nc + ndelta)/totnn; //The payoff that you get from defecting
				countd ++;
				break;
				
			case 2:
				sum = sum + (nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //The payoff that you get from playing delta
				countdelta ++;
				break;
				
			default:
				cout<<"ERROR!!!!!!"<<endl;
				exit(1);
		}
		
		
	}
	
	//Here I save the welfare
	welfare = sum;
	
	//Check that the total number of players sums up to one
	totnn = countc + countd + countdelta;
	if(totnn != cons.N){
		cout<<"ERRORRRR!!! AND IN THE COUNTING"<<endl;
		exit(2);	
	}
	
	//Here I save the percentages
	perc = (double) countc/cons.N;
	perd = (double) countd/cons.N;
	perdelta = (double) countdelta/cons.N;
	
	return;	
}
/*************************************************************************************************************************/
//Move to next round
void nextround(int *oldstrategy, int *newstrategy,Constants cons){
	int i;
	for(i=0;i<cons.N;i++){
		oldstrategy[i]=newstrategy[i];
	}
}
