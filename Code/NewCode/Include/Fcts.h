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

// My includes
#include "Constants.h"



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

