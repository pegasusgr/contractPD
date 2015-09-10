/* 
 * File:  neighbors.cpp 
 * Author: rigosa
 *
 * Created on September 10, 2015, 3:29 PM
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
#include "Include/neighbors.h"

// Gnu Scientific Library (gsl) includes
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h> //Needed for the beta pdf

using namespace std;

int main(){
	// FOR TESTING
	int N=100;
	int L=1;
	int w=sqrt(N);
	int i=9;
	vector<int> neivector=neighbors(i, N, L);
	for(i=0;i<neivector.size();i++){
	cout<<neivector[i]<<endl;
	}	
	return 0;
}
