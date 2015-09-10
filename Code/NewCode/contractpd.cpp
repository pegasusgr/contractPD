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
//#include "contractpd_Constants.h"
//#include "ensemble_Fcts.h"

using namespace std;

class agent {
	int x, y, s;
	public:
	void setplace(int w, int z){x=w; y=z;}
	void setstrategy(int w){s=w;}
	int getx(){return x;}	
	int gety(){return y;}	
	int gets(){return s;}
};


int main() {
	// Start by building the lattice.
	// Lattice width: w
	int w=10;
	// Lattice height: h
	int h=10;
	// Nuber of agents
	int N=w*h;
	agent indiv[N];
	for(int i=0;i<N;i++){
		indiv[i].setplace(i%w,i/w);
		cout << indiv[i].getx()<< endl;
		cout << indiv[i].gety()<< endl;
	}


	return 0;
}
