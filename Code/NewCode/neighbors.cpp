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

// Gnu Scientific Library (gsl) includes
#include<gsl/gsl_rng.h>
#include<gsl/gsl_randist.h> //Needed for the beta pdf

using namespace std;

//int[3] neighbors(int, int, int)
int Wneighbor(int, int);
int Eneighbor(int, int);
int Nneighbor(int, int);
int Sneighbor(int, int);
int NWneighbor(int, int);
int NEneighbor(int, int);
int SWneighbor(int, int);
int SEneighbor(int, int);
vector<int> neighbors(int , int , int );

int main(){
	// FOR TESTING
	int N=100;
	int L=2;
	int w=sqrt(N);
	int i=9;
	vector<int> neivector=neighbors(i, N, L);
	for(i=0;i<neivector.size();i++){
	cout<<neivector[i]<<endl;
	}	
	return 0;
}

// NEIGHBORS takes as inputs i: the agent whose neighbors we are looking at; N: the number of individuals; L: The type of lattice (0 for square von-Neumann, 1 for square Moore, 2 for ring). The output is the identities of the neighbors of player i.
vector<int> neighbors(int i, int N, int L){
	// Initialize
	int nei [3]={0 , 0 , 0};
	int w=-1;
	int neinum=-1; // Number of neighbors
	//Type of lattice.
	switch (L){
	case 0:		// von-Neumann
		w=sqrt(N);
		neinum=4;
		break;
	case 1:		// Moore
		w=sqrt(N);
		neinum=8;
		break;
	case 2:		// Ring
		w=N;
		neinum=2;
		break;
	}
	int j=0;	//index to count neighbors
	vector<int> neiid(neinum,-1);
	switch(L){
	
	case 1: 	// Moore square
		neiid[j]=NWneighbor(i,w);
		j++;
		neiid[j]=NEneighbor(i,w);
		j++;
		neiid[j]=SWneighbor(i,w);
		j++;
		neiid[j]=SEneighbor(i,w);
		j++;
	case 0:		// von-Neumann square
		neiid[j]=Nneighbor(i,w);
		j++;
		neiid[j]=Sneighbor(i,w);
		j++;
	case 2:
		neiid[j]=Eneighbor(i,w);
		j++;
		neiid[j]=Wneighbor(i,w);
	}

	return neiid;
}

int Wneighbor(int i, int w){
	int nei;
	// West neighbor
	if (i%w==0) 
		nei=i+w-1;
	else 
		nei=i-1;
	return nei;
}
int Eneighbor(int i, int w){
	int nei;
       // East neighbor
       if (i%w==w-1)	
	       nei=i-w+1;
       else
	       nei=i+1;
       return nei;
}
int Nneighbor(int i, int w){
	int nei;
       //North neighbor
       if (i/w==0)
	       nei=w*(w-1)+i%w;
       else nei=i-w;
       return nei;
}
int Sneighbor(int i, int w){
	int nei;
       //South neighbor
       if (i/w==w-1)
	       nei=i%w;
       else
	       nei=i+w;
       return nei;
}

int NWneighbor(int i, int w){
	return Nneighbor(Wneighbor(i,w),w);
}

int NEneighbor(int i, int w){
	return Nneighbor(Eneighbor(i,w),w);
}

int SWneighbor(int i, int w){
	return Sneighbor(Wneighbor(i,w),w);
}

int SEneighbor(int i, int w){
	return Sneighbor(Eneighbor(i,w),w);
}

	
