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
		nc=neinum[0];
		nd=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Now I compute the expected utility for each of the strategies
		expected[0]= 1.*(nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //If cooperate: nc*r - nd*a + ndelta*(r+delta)
		expected[1]= 1.*(nc + ndelta)/totnn; //If defect: nc*1 + nd*0 + ndelta*1
		expected[2]= 1.*(nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //If delta: nc*(r-delta) - nd*a + ndelta*r
		
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
		nc=neinum[0];
		nd=neinum[1];
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

/**** This function updates the strategy of each player based on the imitation rule! See guide for more details *****/
void updatestrategyimitation(int *oldstrategy, int *newstrategy, Constants cons, gsl_rng *gslpointer){
	
	int M=3; //The number of strategies (for sake of generalization)
	int i;
	int nc, nd, ndelta, totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	double randomnum; //A random number
	double probarr[M]; //Here I will store the probability for each of the strategies to be played
	double average[M]; //Here I will store the average utility for each of the strategies as it appears to player i
	double sum; //sum variable
	double cumprob[M]; //The array of cumulative probabilities
	double payoff[cons.N]; //The array storing the payoff for each agent at time t-1
	int k; //dummy variable
	vector<int> neiid; //An array that contains the identity of the n.n. of player i
	
	
	//First of all, let's compute the average payoff for each agent i in the round t-i
	for(i=0; i<cons.N; i++){
		//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
		neinum=neighborstrategies(i, oldstrategy, cons);	
		nc=neinum[0];
		nd=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Now I compute the payoff for agent i:
		switch(oldstrategy[i]){
			
			case 0:
				payoff[i] = 1.*(nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //The payoff that you get from cooperating
				break;
			
			case 1:
				payoff[i] = 1.*(nc + ndelta)/totnn; //The payoff that you get from defecting
				break;
				
			case 2:
				payoff[i] = 1.*(nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //The payoff that you get from playing delta
				break;
				
			default:
				cout<<"ERROR!!!!!!"<<endl;
				exit(3);
		}
	}
	
	// Now let's see what is the new strategy for agent i
	for(i=0; i< cons.N; i++){
		
		//First of all, let's see who are the n.n. of player i
		neiid = neighbors(i, cons);
		//and let's see how many play each strategy:
		neinum=neighborstrategies(i, oldstrategy, cons);	
		nc=neinum[0];
		nd=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		
		/*******Now let's compute the payoff that i sees for each strategy (including himself) ****************************/
		for(k=0; k< M; k++){ average[k]=0;} //Set the average array to zero
		for(k=0; k<totnn; k++){
			switch(oldstrategy[neiid[k]]){
			
			case 0:
				average[0] = average[0] + payoff[neiid[k]]; //Because neiid[k] is a coop
				break;
			
			case 1:
				average[1] = average[1] + payoff[neiid[k]]; //Because neiid[k] is a def
				break;
				
			case 2:
				average[2] = average[2] + payoff[neiid[k]]; //Because neiid[k] is playing delta
				break;
				
			default:
				cout<<"ERROR in assigning the average payoffs !!!!!!"<<endl;
				exit(3);
			}
		
		}
		//Now we add the strategy of agent i itself
		switch(oldstrategy[i]){
			
			case 0:
				average[0] = average[0] + payoff[i]; //Because i is a coop
				nc++;
				break;
			
			case 1:
				average[1] = average[1] + payoff[i]; //Because i is a def
				nd++; 
				break;
				
			case 2:
				average[2] = average[2] + payoff[i]; //Because i is playing delta
				ndelta++;
				break;
				
			default:
				cout<<"ERROR in assigning the average payoffs !!!!!!"<<endl;
				exit(3);
		}
		
		/******Now I renormalize the payoffs and compute the probabilities for each of the strategies*****/
		sum = 0;
		if(nc == 0){probarr[0]=0;} //Yeah, here it could have been done more elegantly with a bool array, but it's only 3 strategies anyway!
		else{
			probarr[0] = exp(cons.beta*(average[0]/nc)); //The logit prob
			sum = sum  + probarr[0];
		}
		if(nd == 0){probarr[1]=0;}
		else{
			probarr[1] = exp( cons.beta*(average[1]/nd) ); //The logit prob
			sum = sum + probarr[1];
		}
		if(ndelta==0){probarr[2]=0;}
		else{
			probarr[2] = exp(cons.beta*(average[2]/ndelta)); //The logit prob
			sum = sum + probarr[2];
		}
		//Here I renormalize the probability but I have to do the first by hand due to cumprob
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
/**************************************************************************************************************/

/************ This function receives an integers as input as returns one of the other two strategies with a 50% probability *************************/
int flipcoinewstrategy(int k, gsl_rng *gslpointer){
	int i;
	int M=3; //The number of strategies
	double randnum;
	
	randnum = gsl_ran_flat(gslpointer,0,1);
	if(randnum < 0.5){ //With 50% probability change the strategy to i+1; if i+1 is M, then the strategy is 0. Periodic boundary conditions on 0,1,2.
		i = k + 1; 
		if(i >=M){
			i=0;
		}
	}
	else{ //With 50% probability change the strategy to i-1; if i+1 is -1, then the strategy is M. Periodic boundary conditions on 0,1,2.
		i = k - 1;
		if(i < 0){
			i=M-1;	
		}
	} 
	
	return i;
}

/******************* Here I just compute the probability of somebody spotaneously mutating ****************************/
void changeformutation(int *strategy, Constants cons, gsl_rng *gslpointer){
	int i;
	double randnum;
	
	for(i=0; i<cons.N; i++){
		randnum = gsl_ran_flat(gslpointer,0,1); //Generate a random number btw 0 and 1.
		if(randnum < cons.mu){ //Check if agent i is mutating
			strategy[i] = flipcoinewstrategy(strategy[i], gslpointer); //If yes, get the new strategy
		}
	}
	
	return ;
}





/****************************************************************************************************************************************************************/
/******************** NEW STUFF STARTS HERE !!! **************************************/
/**************************************************************************************************************************************************************************/

/****** This function updates the payoffs in 2 different ways: with probabilty p, an agent updates with miopic best response and with probabilty 1-p he updates imitating other people ***/
void updatestrategyboth(int *oldstrategy, int *newstrategy, Constants cons, gsl_rng *gslpointer, double p){
	
	int M=3; //The number of strategies (for sake of generalization)
	int i;
	int nc, nd, ndelta, totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	double randomnum; //A random number
	double probarr[M]; //Here I will store the probability for each of the strategies to be played
	double average[M]; //Here I will store the average utility for each of the strategies as it appears to player i
	double sum; //sum variable
	double cumprob[M]; //The array of cumulative probabilities
	double payoff[cons.N]; //The array storing the payoff for each agent at time t-1
	int k; //dummy variable
	vector<int> neiid; //An array that contains the identity of the n.n. of player i
	double expected[M]; //Here I will store the expected utility for each of the strategies
	
	
	//First of all, let's compute the average payoff for each agent i in the round t-i
	for(i=0; i<cons.N; i++){
		//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
		neinum=neighborstrategies(i, oldstrategy, cons);	
		nc=neinum[0];
		nd=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Now I compute the payoff for agent i:
		switch(oldstrategy[i]){
			
			case 0:
				payoff[i] = 1.*(nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //The payoff that you get from cooperating
				break;
			
			case 1:
				payoff[i] = 1.*(nc + ndelta)/totnn; //The payoff that you get from defecting
				break;
				
			case 2:
				payoff[i] = 1.*(nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //The payoff that you get from playing delta
				break;
				
			default:
				cout<<"ERROR!!!!!!"<<endl;
				exit(3);
		}
	}
	
	// Now let's see what is the new strategy for agent i. Here the probabilty p comes into play
	for(i=0; i< cons.N; i++){
		
		randomnum = gsl_ran_flat(gslpointer,0,1);
		
		if(randomnum < p){ //Here I update with miopic best response
			//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
			neinum=neighborstrategies(i, oldstrategy, cons);	
			nc=neinum[0];
			nd=neinum[1];
			ndelta=neinum[2];			
			totnn = nc + nd + ndelta; //Compute the total number of neighbours
			//Now I compute the expected utility for each of the strategies
			expected[0]= 1.*(nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //If cooperate: nc*r - nd*a + ndelta*(r+delta)
			expected[1]= 1.*(nc + ndelta)/totnn; //If defect: nc*1 + nd*0 + ndelta*1
			expected[2]= 1.*(nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //If delta: nc*(r-delta) - nd*a + ndelta*r
			
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
		
		else{ //Here the player updates with imitation
			//First of all, let's see who are the n.n. of player i
			neiid = neighbors(i, cons);
			//and let's see how many play each strategy:
			neinum=neighborstrategies(i, oldstrategy, cons);	
			nc=neinum[0];
			nd=neinum[1];
			ndelta=neinum[2];			
			totnn = nc + nd + ndelta; //Compute the total number of neighbours
			
			/*******Now let's compute the payoff that i sees for each strategy (including himself) ****************************/
			for(k=0; k< M; k++){ average[k]=0;} //Set the average array to zero
			for(k=0; k<totnn; k++){
				switch(oldstrategy[neiid[k]]){
				
				case 0:
					average[0] = average[0] + payoff[neiid[k]]; //Because neiid[k] is a coop
					break;
				
				case 1:
					average[1] = average[1] + payoff[neiid[k]]; //Because neiid[k] is a def
					break;
					
				case 2:
					average[2] = average[2] + payoff[neiid[k]]; //Because neiid[k] is playing delta
					break;
					
				default:
					cout<<"ERROR in assigning the average payoffs !!!!!!"<<endl;
					exit(3);
				}
			
			}
			//Now we add the strategy of agent i itself
			switch(oldstrategy[i]){
				
				case 0:
					average[0] = average[0] + payoff[i]; //Because i is a coop
					nc++;
					break;
				
				case 1:
					average[1] = average[1] + payoff[i]; //Because i is a def
					nd++; 
					break;
					
				case 2:
					average[2] = average[2] + payoff[i]; //Because i is playing delta
					ndelta++;
					break;
					
				default:
					cout<<"ERROR in assigning the average payoffs !!!!!!"<<endl;
					exit(3);
			}
				
			/******Now I renormalize the payoffs and compute the probabilities for each of the strategies*****/
			sum = 0;
			if(nc == 0){probarr[0]=0;} //Yeah, here it could have been done more elegantly with a bool array, but it's only 3 strategies anyway!
			else{
				probarr[0] = exp(cons.beta*(average[0]/nc)); //The logit prob
				sum = sum  + probarr[0];
			}
			if(nd == 0){probarr[1]=0;}
			else{
				probarr[1] = exp( cons.beta*(average[1]/nd) ); //The logit prob
				sum = sum + probarr[1];
			}
			if(ndelta==0){probarr[2]=0;}
			else{
				probarr[2] = exp(cons.beta*(average[2]/ndelta)); //The logit prob
				sum = sum + probarr[2];
			}
			//Here I renormalize the probability but I have to do the first by hand due to cumprob
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
	
	}
	
	return ;	
}
/**************************************************************************************************************/

/*************** This function updates the strategies of each player. 
Instead of updating once each player, I randomly update N times in sequence!!! This means that the active player is actually best responding to things that players are playing now

This could change everything*/
/****************** Function that updates other people strategy *************************/
void updatestrategyoneatthetime(int *oldstrategy, int *newstrategy, Constants cons, gsl_rng *gslpointer){
	
	int M=3; //The number of strategies (for sake of generalization)
	int i;
	int j;
	int nc, nd, ndelta, totnn; //Number of n.n. playing each strategy and total number of n.n.
	vector<int> neinum;	//The array that contains the number of n.n. playing each strategy.
	double randomnum; //A random number
	double probarr[M]; //Here I will store the probability for each of the strategies to be played
	double expected[M]; //Here I will store the expected utility for each of the strategies
	double sum; //sum variable
	double cumprob[M]; //The array of cumulative probabilities
	int k; //dummy variable
	
	//Here I update the strategy for each of the agents

	for(j=0; j< cons.N; j++){
		
		i = gsl_rng_uniform_int (gslpointer, cons.N); //Here I randomly sample which agent I'm updating at the moment!
	
		//getnumber of each thing (as a fct of i, oldstrategy and the type of lattice);
		neinum=neighborstrategies(i, oldstrategy, cons);	
		nc=neinum[0];
		nd=neinum[1];
		ndelta=neinum[2];			
		totnn = nc + nd + ndelta; //Compute the total number of neighbours
		//Now I compute the expected utility for each of the strategies
		expected[0]= 1.*(nc*cons.r -nd*cons.a +ndelta*(cons.r+cons.delta))/totnn; //If cooperate: nc*r - nd*a + ndelta*(r+delta)
		expected[1]= 1.*(nc + ndelta)/totnn; //If defect: nc*1 + nd*0 + ndelta*1
		expected[2]= 1.*(nc*(cons.r-cons.delta) -nd*cons.a + ndelta*cons.r)/totnn; //If delta: nc*(r-delta) - nd*a + ndelta*r
		
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
		oldstrategy[i] = k; //Here I am cheating to make a quick and dirty study. I'm updating also oldstrategy for the active player, so that I have everything up to date
	}
	
	return ;
}
/*********************************************************************************************************/
