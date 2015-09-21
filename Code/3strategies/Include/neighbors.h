
/* 
 * File:  neighbors.h 
 * Author: rigosa
 *
 * Created on September 10, 2015, 6:11 PM
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

// Include the Constants class
#include "Constants.h"

using namespace std;
// Very minor functions. Find at the end of the file
int Wneighbor(int, int);
int Eneighbor(int, int);
int Nneighbor(int, int);
int Sneighbor(int, int);
int NWneighbor(int, int);
int NEneighbor(int, int);
int SWneighbor(int, int);
int SEneighbor(int, int);

int latwidth(Constants cons){ //get the width of the lattice
	int N=cons.N;
	int L=cons.L;
	int w=-1;
	switch (L){
		case 0:
		case 1:
			w=sqrt(N);
			break;
		case 2:
			w=N;	// If we're on a ring, then width is the full length of the ring 
			break;
	}
	return w;
}

int neinumber(Constants cons){ //get the number of neighbors
	int n=-1;
	switch (cons.L){
		case 0:
			n=4;	// von-Neumann
			break;
		case 1:
			n=8;	// Moore
			break;
		case 2:
			n=2;	// Ring
	}
	return n;
}


// NEIGHBORS takes as inputs i: the agent whose neighbors we are looking at; N: the number of individuals; L: The type of lattice (0 for square von-Neumann, 1 for square Moore, 2 for ring). The output is a vector with the identities of the neighbors of player i.
vector<int> neighbors(int i, Constants cons){
	int L=cons.L;
	// Initialize
	int w = latwidth(cons);
	int neinum = neinumber(cons); // Number of neighbors
	int j=0;	//index to count neighbors
	vector<int> neiid(neinum,-1);	// I am using a vector because of the variable size of the number of neighbors.
	switch(L){	// I add more neighbors as we go on. Notice that there is no break command between cases.
	
	case 1: 	// Moore square. (Add ``corner'' neighbors)
		neiid[j]=NWneighbor(i,w);
		j++;
		neiid[j]=NEneighbor(i,w);
		j++;
		neiid[j]=SWneighbor(i,w);
		j++;
		neiid[j]=SEneighbor(i,w);
		j++;
	case 0:		// von-Neumann square. (Add North and South neighbors)
		neiid[j]=Nneighbor(i,w);
		j++;
		neiid[j]=Sneighbor(i,w);
		j++;
	case 2:		// Ring. (Add East and West neighbors)
		neiid[j]=Eneighbor(i,w);
		j++;
		neiid[j]=Wneighbor(i,w);
	}

	return neiid;
}

vector<int> neighborstrategies(int i, int *strategyarray, Constants cons){
	unsigned int j;			// Dummy
	int M=3;		// Number of strategies
	vector<int> stratnum(M,0);	// This will hold the number of neighbors of each strategy
	vector<int> neivec=neighbors(i, cons);
	for(j=0 ; j<neivec.size() ; j++){
		stratnum[strategyarray[neivec[j]]]++;	// Add 1 to the relevant strategy type
	}
	return stratnum;
}

// The following functions find specific neighbors of player i given the width of the lattice w.
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
// NorthWest neighbor
int NWneighbor(int i, int w){
	return Nneighbor(Wneighbor(i,w),w);
}

// NorthEast neighbor
int NEneighbor(int i, int w){
	return Nneighbor(Eneighbor(i,w),w);
}

// SouthWest neighbor
int SWneighbor(int i, int w){
	return Sneighbor(Wneighbor(i,w),w);
}

// SouthEast neighbor
int SEneighbor(int i, int w){
	return Sneighbor(Eneighbor(i,w),w);
}

	
