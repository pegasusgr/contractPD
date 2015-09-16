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
#include<numeric>


// Gnu Scientific Library (gsl) includes
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h> //Needed for the beta pdf

// My includes
#include "Constants.h"
#include "neighbors.h"



using namespace std;
double game(int, int, int,Constants);
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

void printstuffsingleloop(ofstream& filet, ofstream& fileag, int t, double welfare, double perc, double perd, double pergen, double peropp, int *strategy ,Constants cons){
	
	int i; //dummy variable
	
    filet<<t<<" "<<welfare<<" "<<perc<<" "<<perd<<" "<<pergen<<" "<<peropp<<endl; //Here I print time, welfare and percentages.
	
    for(i=0;i<cons.N;i++){ //Here I print the strategy for each agent at each time step and.
        fileag<<strategy[i]<<" ";
    }
    
    fileag<<endl;
 
    return;
}
/**************************************************************************/

/*********************************************/
int binaryprobsearch(vector<double> Gamma, int M, double x) { //Binary search. Gamma is the array of cumulative prob. M is the lenght of the array and x is the random number between zero and one
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

/********************* Function that checks whether type i can transition to type j***********/
int transit(int i, int j, Constants cons){
	int trans;
	trans=cons.transition[cons.M*i+j];
	return trans;
}
/****************** Function that updates other people strategy *************************/
void updatestrategy(int *oldstrategy, int *newstrategy, Constants cons, gsl_rng *gslpointer){
//cout<<"in updatestrategy"<<endl;	
	int M=cons.M; //The number of strategies (for sake of generalization)
	int i,j;
	int  totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	double randomnum; //A random number
	vector<double> provec(M,0); //Here I will store the probability for each of the strategies to be played
	double expected[M]; //Here I will store the expected utility for each of the strategies
	double sum; //sum variable
	vector<double> cumprob(M,0); //The array of cumulative probabilities
	int k; //dummy variable
	
	//Here I update the strategy for each of the agents

	for(i=0; i< cons.N; i++){
		//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
		neinum=neighborstrategies(i, oldstrategy, cons);	
		totnn=0;
		for (j=0;j<M;j++){
			totnn = totnn + neinum[j];
		}
//cout<<"in updatestrategy, player "<<i<<endl;	
		
		//Now I compute the expected utility for each of the strategies
		for(j=0;j<M;j++){
			expected[j]=0;
			for (k=0;k<M;k++){
				//if(transit(oldstrategy[i],j,cons)){
					expected[j] = expected[j] + neinum[k]*game(j,k,oldstrategy[i],cons);
				//}
			}
			expected[j]=expected[j]/totnn;
		}
		
		/******Now I compute the probabilities for each of the strategies*****/
		sum = 0;
		for(k=0; k < M; k++){
			provec[k]=0;
			if(transit(oldstrategy[i],k,cons)){
				provec[k]=(exp(cons.beta * expected[k]));
			}
		}
		sum= accumulate(provec.begin(), provec.end(), 0.0);
		//Here I renormalize but I have to do the first by hand due to cumprob
		provec[0] = provec[0]/sum;
		cumprob[0]=provec[0];
		//cout<<"Probabilities:"<< cumprob[0]<<" ";
		for(k=1; k <M; k++){
			provec[k] = provec[k]/sum;
			cumprob[k]=(cumprob[k-1] + provec[k]);
			//cout<<cumprob[k]<<" ";
		}
		//cout<<endl;
		
		// Now I sample the random number:
		randomnum = gsl_ran_flat(gslpointer,0,1); //Generate a random number btw 0 and 1. Note that I use utility here just to not use another variable
    	k=binaryprobsearch(cumprob,M,randomnum); //Here I compute which strategy is the agent i is using. Note that I use k here just to not use another variable
	//cumprob.clear();
	//provec.clear();
		//Hence:
		newstrategy[i] = k;
	}
	
	return ;
}
/*********************************************************************************************************/

/****************Function that computes the welfare and the percentage of each strategy on the lattice *********/
void computetotalwelfare(int *newstrategy, double &welfare, double &perc, double &perd, double &pergen, double &peropp,Constants cons){
	int i,j;
	double sum; //sum variable
	int totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	int countn[4]={}; // Integers for counting
	
	
	sum = 0;
	
	for(i=0; i< cons.N; i++){
		//getnumber of each thing (as a fct of i, newstrategy and the type of lattice);
		neinum=neighborstrategies(i, newstrategy, cons);	
		//Check which strategy the player is using and update the utility accordingly
		for(j=0;j<4;j++){
			sum = sum + neinum[j]*game(newstrategy[i],j,j,cons); // Compute utility TODO
		}
			countn[newstrategy[i]]++; // Count the number of each of the strategies
	}
	
	//Here I save the welfare
	welfare = sum;
	totnn=0;	
	//Check that the total number of players sums up to one
	for(j=0;j<4;j++){
		totnn = totnn + countn[j];
	}
	if(totnn != cons.N){
		cout<<"ERRORRRR!!! AND IN THE COUNTING"<<endl;
		exit(2);	
	}
	
	//Here I save the percentages
	perc = (double) countn[0]/cons.N;
	perd = (double) countn[1]/cons.N;
	pergen = (double) countn[2]/cons.N;
	peropp = (double) countn[3]/cons.N;
	
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


/**************** This function takes as inputs my strategy and the other player's strategy (and parameters) and returns my payoff. *********/
double game(int mystrategy, int otherstrategy, int myoldstrategy,Constants cons){
	double payoff=-10;
	int defect=(int)(myoldstrategy==1);
	defect=1;
	switch (mystrategy){
		case 0:	// I am Cooperator
			switch (otherstrategy){
				case 0:	// Other player is Cooperator
					payoff=cons.r;
					break;
				case 1:	// Other player is Defector
					payoff=-cons.a;
					break;
				case 2:	// Other player is Generous
					payoff=cons.r+defect*cons.delta;
					break;
				case 3:	// Other player is Opportunistic
					payoff=cons.r-cons.delta;
					break;
			}
			break;
		case 1: // I am Defector
			switch (otherstrategy){
				case 0:	// Other player is Cooperator
					payoff=1.;
					break;
				case 1:	// Other player is Defector
					payoff=0.;
					break;
				case 2:	// Other player is Generous
					payoff=1.;
					break;
				case 3:	// Other player is Opportunistic
					payoff=1.;
					break;
			}
			break;
		case 2: // I am Generous
			switch (otherstrategy){
				case 0:	// Other player is Cooperator
					payoff=cons.r-cons.delta;
					break;
				case 1:	// Other player is Defector
					payoff=-cons.a;
					break;
				case 2:	// Other player is Generous
					payoff=cons.r;
					break;
				case 3:	// Other player is Opportunistic
					payoff=cons.r-2*cons.delta;
					break;
			}
			break;
		case 3: // I am Opportunistic
			switch (otherstrategy){
				case 0:	// Other player is Cooperator
					payoff=cons.r+cons.delta;
					break;
				case 1:	// Other player is Defector
					payoff=-cons.a;
					break;
				case 2:	// Other player is Generous
					payoff=cons.r+2*cons.delta;
					break;
				case 3:	// Other player is Opportunistic
					payoff=cons.r;
					break;
			}
			break;
	}
	return payoff;
}
