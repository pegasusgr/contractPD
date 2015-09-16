/* 
 * File:   Constants.cpp
 * Author: sduca
 * 
 * Created on February 4, 2015, 2:06 PM
 */


#include<cstdio>
#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<fstream>
#include<vector>
#include "Constants.h"

using namespace std;

Constants::Constants(){ //Note that name must be the entire path; i.e. "./config.conf"
	char line[256];
	int linenum=0;
	int count=0, p=8; //M is the amount of parameters I have to give, count will range from 0 to M-1
	double vector[p]; //will store the M parameters
	FILE *pfile;

	pfile = fopen ("./config.conf" , "r");

	while(fgets(line, 256, pfile) != NULL)
	{
		linenum++;
		if(line[0] == '#') {continue;} //I'm going to the next line without reading and incrementing count
		sscanf(line, "%*s %lf", &vector[count]);
		count ++;
	}
	fclose(pfile);
	
	//Here I cast when I actually have integers
	N=int(vector[0]); //Number of agents
	T=int(vector[1]);//Number of time steps
	L=int(vector[2]); //Type of lattice: 0 for Von Neumann square lattice; 1 for Moore square lattice; 2 for ring
	delta=vector[3]; //value of the contract
	beta=vector[4]; //beta value for the logit
	r=vector[5]; //cooperation reward
    	a=vector[6]; //sucker's payoff
    	M=int(vector[7]); //number of strategies
	cout<<M<<endl;
	// Read the transition matrix
	//transition= malloc (M*M*sizeof(int));
	transition= new int[M*M];
	pfile = fopen ("./transition.conf" , "r");
	count=0;
	while(fgets(line, 256, pfile) != NULL)
	{
		if(line[0] == '#') {continue;} //I'm going to the next line without reading and incrementing count
		if(M==3){
		sscanf(line,"%d %d %d", &transition[count], &transition[count+1], &transition[count+2]);
		}
		else if(M == 4){
		sscanf(line,"%d %d %d %d", &transition[count], &transition[count+1], &transition[count+2], &transition[count+3]);
		}
		count=count+M;
		//cout<<"count is: "<<count<<endl;
		//cout<<"transition["<<count-1<<"] is: "<<transition[count-1]<<endl;
	}
	fclose(pfile);
}
Constants::~Constants(){}
